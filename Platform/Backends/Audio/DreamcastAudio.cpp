#include "DreamcastAudio.h"

//#include "AudioMixer.h"
#include "MediaCatalog.h"

#include <kos.h>
#include <dc/sound/sound.h>
#include <dc/sound/aica_comm.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

extern char* FullPath(char* file);

static constexpr int MIX_FREQUENCY = 48000;
static constexpr int MIX_CHANNELS  = 2;
static constexpr int MIX_BUFFER_SAMPLES = 8192;

struct SfxState
{
    SoundHandle id = 0;
    int chn = -1;
    uint64_t count = 0;
    
    int32_t volume;
    int32_t pan;
    float left;
    float right;
    uint8_t aicaVol;
    uint8_t aicaPan;
};

struct MovieState
{
    void* Converter = nullptr;
};

static MovieState Movie;
static std::vector<SfxState> SfxStateVec;
static uint64_t g_PlayCounter = 0;

static inline void RecomputeAICA(SfxState& v)
{
    // clamp inputs
    int32_t volume = std::clamp<int32_t>(v.volume, -4000, 0);
    int32_t pan    = std::clamp<int32_t>(v.pan, -1000, 1000);

    // equal-power gain
    float gain = (volume + 4000) / 4000.0f;

    float p = pan / 1000.0f;
    constexpr float quarterPi = 0.78539816339f;

    float angle = (p + 1.0f) * quarterPi;

    v.left  = gain * std::cos(angle);
    v.right = gain * std::sin(angle);

    // map back to AICA volume, preserving perceived loudness
    float mono = std::sqrt(v.left * v.left + v.right * v.right);
    v.aicaVol = static_cast<uint8_t>(std::clamp(mono, 0.0f, 1.0f) * 255.0f);

    // derive AICA panning from balance
    float sum = v.left + v.right;
    float bal = (sum > 1e-6f) ? (v.right / sum) : 0.5f;

    v.aicaPan = static_cast<uint8_t>(std::clamp(bal, 0.0f, 1.0f) * 255.0f);
}

DreamcastAudio::DreamcastAudio() = default;

DreamcastAudio::~DreamcastAudio()
{
    Destroy();
}

bool DreamcastAudio::Init()
{
    snd_init();
    return true;
}

void DreamcastAudio::Destroy()
{
    StopMusic();
    Reset();
}

SoundHandle DreamcastAudio::CreateSound(int soundId, int maxPolyphony)
{
    if (!Init())
    {
        return 0;
    }

    const char* name = GetSoundEffect(soundId);
    if (name == nullptr)
    {
        return 0;
    }

    char relBuf[512];
    std::snprintf(relBuf, sizeof(relBuf), "audio/%s", name);
    const char* path = FullPath(relBuf);

    printf("%s: Loading %s\n", __func__, path);
    sfxhnd_t handle = snd_sfx_load(path);
    if (handle == SFXHND_INVALID)
    {
        printf("Couldn't load SFX %s\n", path);
        return 0;
    }

    return reinterpret_cast<SoundHandle>(handle);
    return 0;
}

void DreamcastAudio::DestroySound(SoundHandle sound)
{
    snd_sfx_unload(reinterpret_cast<sfxhnd_t>(sound));
}

void DreamcastAudio::PlayOneShot(SoundHandle sound, int32_t volume, int32_t pan)
{
    sfx_play_data_t sfx_data = {0};
    SfxState state{};

    state.volume = volume;
    state.pan = pan;
    RecomputeAICA(state);

    sfx_data.chn = -1;
    sfx_data.idx = reinterpret_cast<sfxhnd_t>(sound);
    sfx_data.vol = state.aicaVol;
    sfx_data.pan = state.aicaPan;
    sfx_data.loop = 0;

    state.id = sound;
    state.chn = snd_sfx_play_ex(&sfx_data);
    state.count = ++g_PlayCounter;
    SfxStateVec.push_back(state);
}

void DreamcastAudio::PlayLoop(SoundHandle sound, int32_t volume, int32_t pan)
{
    sfx_play_data_t sfx_data = {0};
    SfxState state{};

    state.volume = volume;
    state.pan = pan;
    RecomputeAICA(state);

    sfx_data.chn = -1;
    sfx_data.idx = reinterpret_cast<sfxhnd_t>(sound);
    sfx_data.vol = state.aicaVol;
    sfx_data.pan = state.aicaPan;
    sfx_data.loop = 1;

    state.id = sound;
    state.chn = snd_sfx_play_ex(&sfx_data);
    state.count = ++g_PlayCounter;
    SfxStateVec.push_back(state);
}

void DreamcastAudio::StopSound(SoundHandle sound)
{
    for (auto it = SfxStateVec.begin(); it != SfxStateVec.end(); )
    {
        if (it->id == sound)
        {
            snd_sfx_stop(it->chn);
            it = SfxStateVec.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void DreamcastAudio::StopCurrent(SoundHandle sound)
{
    auto newestIt = SfxStateVec.end();

    for (auto it = SfxStateVec.begin(); it != SfxStateVec.end(); ++it)
    {
        if (it->id == sound)
        {
            if (newestIt == SfxStateVec.end() || it->count > newestIt->count)
            {
                newestIt = it;
            }
        }
    }

    if (newestIt != SfxStateVec.end())
    {
        snd_sfx_stop(newestIt->chn);
        SfxStateVec.erase(newestIt);
    }
}

void DreamcastAudio::SetVolume(SoundHandle sound, int32_t volume)
{
    for (auto it = SfxStateVec.begin(); it != SfxStateVec.end(); )
    {
        if (it->id == sound)
        {
            it->volume = volume;
            RecomputeAICA(*it);
    
            AICA_CMDSTR_CHANNEL(tmp, cmd, chan);
            cmd->cmd = AICA_CMD_CHAN;
            cmd->timestamp = 0;
            cmd->size = AICA_CMDSTR_CHANNEL_SIZE;
            cmd->cmd_id = it->chn;
            chan->cmd = AICA_CH_CMD_UPDATE | AICA_CH_UPDATE_SET_VOL;
            chan->vol = it->aicaVol;
            snd_sh4_to_aica(tmp, cmd->size);
        }
        ++it;
    }
}

void DreamcastAudio::SetPan(SoundHandle sound, int32_t pan)
{
    for (auto it = SfxStateVec.begin(); it != SfxStateVec.end(); )
    {
        if (it->id == sound)
        {
            it->pan = pan;
            RecomputeAICA(*it);

            AICA_CMDSTR_CHANNEL(tmp, cmd, chan);
            cmd->cmd = AICA_CMD_CHAN;
            cmd->timestamp = 0;
            cmd->size = AICA_CMDSTR_CHANNEL_SIZE;
            cmd->cmd_id = it->chn;
            chan->cmd = AICA_CH_CMD_UPDATE | AICA_CH_UPDATE_SET_PAN;
            chan->pan = it->aicaPan;
            snd_sh4_to_aica(tmp, cmd->size);
        }
        ++it;
    }
}

void DreamcastAudio::Reset()
{
    CloseMovieStream();

    printf("TODO: %s\n", __func__);
}

bool DreamcastAudio::PlayMusic(int trackNumber)
{
    // On Dreamcast we go back to CDDA, freeing AICA to do SFX stuff later
    // TODO: Error handling

    const MusicTrack* track = GetMusicTrack(trackNumber);
    if (track == nullptr)
    {
        return false;
    }
    
    int volume = (std::sqrt(track->Volume) * 15);// (track->Volume * 15);
    printf("music volume: %d\n", volume);
    cdrom_cdda_play((trackNumber - 1), (trackNumber - 1), 15, CDDA_TRACKS);
    spu_cdda_volume(volume, volume);

    return true;
}

void DreamcastAudio::StopMusic()
{
    cdrom_cdda_pause();
}

bool DreamcastAudio::OpenMovieStream(int sampleRate, int channels)
{
    CloseMovieStream();

    if (sampleRate <= 0 || channels <= 0 || channels > 2)
    {
        return false;
    }

    if (!Init())
    {
        return false;
    }

    printf("TODO: %s\n", __func__);

    return true;
}

void DreamcastAudio::SubmitMovieAudio(const float* samples, int sampleCount)
{
    if (Movie.Converter == nullptr || samples == nullptr || sampleCount <= 0)
    {
        return;
    }

    const int bytes = sampleCount * static_cast<int>(sizeof(float));

    printf("TODO: %s\n", __func__);
}

void DreamcastAudio::CloseMovieStream()
{
    printf("TODO: %s\n", __func__);
}
