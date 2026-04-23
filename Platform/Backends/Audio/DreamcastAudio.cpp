#include "DreamcastAudio.h"

#include "AudioMixer.h"
#include "MediaCatalog.h"
#include "MP3MusicTrack.h"

#include <kos.h>

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
static constexpr int MUSIC_DECODE_BATCH_FRAMES = 4096;
static constexpr int MUSIC_DECODE_SCRATCH_SAMPLES = MUSIC_DECODE_BATCH_FRAMES * 2; // worst-case stereo

struct SfxState
{
    void* Stream = nullptr;
    uint8_t Spec = {};
    bool Ready = false;
};

struct MusicState
{
    void* Converter = nullptr;
    MP3MusicTrack Source;
    float Gain = 1.0f;
    bool Active = false;
};

struct MovieState
{
    void* Converter = nullptr;
};

static SfxState Sfx;
static AudioMixer SfxMixer;
static MusicState Music;
static MovieState Movie;

static float MixBuffer[MIX_BUFFER_SAMPLES];

static float MusicDecodeScratch[MUSIC_DECODE_SCRATCH_SAMPLES];
static float MixedAudioScratch[MIX_BUFFER_SAMPLES];

static bool LoadWav(const char* path, std::vector<float>& outSamples, int& outFrameCount)
{
    printf("TODO: %s\n", __func__);

    return false;
}

static void CleanUpMusicStuff(MusicState& musicState)
{
    printf("TODO: %s\n", __func__);
    musicState.Converter = nullptr;
    musicState.Source.Close();
    musicState.Active = false;
}

static void FeedMusicConverter(void* /*UserData*/, void* stream,
                                       int AdditionalAmount, int /*TotalAmount*/)
{
    if (!Music.Active || !Music.Source.IsOpen())
    {
        return;
    }

    const int channels = Music.Source.GetChannelCount();
    const int bytesPerFrame = channels * static_cast<int>(sizeof(float));
    if (bytesPerFrame <= 0 || AdditionalAmount <= 0)
    {
        return;
    }

    const int requestedFrames = (AdditionalAmount + bytesPerFrame - 1) / bytesPerFrame;
    const int framesToRead = std::min(requestedFrames, MUSIC_DECODE_BATCH_FRAMES);

    const size_t framesRead = Music.Source.ReadFrames(MusicDecodeScratch, static_cast<size_t>(framesToRead));
    if (framesRead == 0)
    {
        return;
    }

    const int bytes = static_cast<int>(framesRead) * channels * static_cast<int>(sizeof(float));
    printf("TODO: %s\n", __func__);
}

static void MixMusic(float* dst, int sampleCount)
{
    if (!Music.Active || Music.Converter == nullptr)
    {
        return;
    }

    const int neededBytes = sampleCount * static_cast<int>(sizeof(float));
    printf("TODO: %s\n", __func__);
    const int got = 0;
    if (got <= 0)
    {
        return;
    }

    const int gotSamples = got / static_cast<int>(sizeof(float));
    const float gain = Music.Gain;
    for (int i = 0; i < gotSamples; i++)
    {
        dst[i] += MixedAudioScratch[i] * gain;
    }
}

static void MixMovie(float* dst, int sampleCount)
{
    if (Movie.Converter == nullptr)
    {
        return;
    }

    const int neededBytes = sampleCount * static_cast<int>(sizeof(float));

    printf("TODO: %s\n", __func__);
    const int got = 0;

    if (got <= 0)
    {
        return;
    }

    const int gotSamples = got / static_cast<int>(sizeof(float));
    for (int i = 0; i < gotSamples; i++)
    {
        dst[i] += MixedAudioScratch[i];
    }
}

static void FeedSfx(void* /*UserData*/, void* stream, int additionalAmount, int /*TotalAmount*/)
{
    int sampleCount = additionalAmount / static_cast<int>(sizeof(float));
    if (sampleCount > MIX_BUFFER_SAMPLES)
    {
        sampleCount = MIX_BUFFER_SAMPLES;
    }
    const int byteCount = sampleCount * static_cast<int>(sizeof(float));

    std::memset(MixBuffer, 0, byteCount);

    MixMusic(MixBuffer, sampleCount);
    MixMovie(MixBuffer, sampleCount);

    SfxMixer.MixVoicesInto(MixBuffer, sampleCount);

    printf("TODO: %s\n", __func__);

    for (int i = 0; i < sampleCount; i++)
    {
        MixBuffer[i] = 0;
    }

}

static SoundAsset* ToAsset(SoundHandle handle)
{
    return reinterpret_cast<SoundAsset*>(handle);
}

DreamcastAudio::DreamcastAudio()
{
    printf("Hello, %s!\n", __func__);
}

DreamcastAudio::~DreamcastAudio()
{
    Destroy();
}

bool DreamcastAudio::Init()
{
    if (Sfx.Ready)
    {
        return true;
    }

    printf("TODO: %s\n", __func__);

    Sfx.Ready = false;
    return true;
}

void DreamcastAudio::Destroy()
{
    StopMusic();
    Reset();
    printf("TODO: %s\n", __func__);
    Sfx.Stream = nullptr;
    if (Sfx.Ready)
    {
        Sfx.Ready = false;
    }
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

    std::unique_ptr<SoundAsset> asset(new SoundAsset());
    asset->SoundId = soundId;
    asset->MaxPolyphony = std::max(1, maxPolyphony);

    char relBuf[512];
    std::snprintf(relBuf, sizeof(relBuf), "audio/%s", name);
    const char* path = FullPath(relBuf);
    if (!LoadWav(path, asset->Samples, asset->FrameCount))
    {
        return 0;
    }

    return reinterpret_cast<SoundHandle>(asset.release());
}

void DreamcastAudio::DestroySound(SoundHandle sound)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);
    SfxMixer.DeactivateVoicesFor(asset);

    delete asset;
}

void DreamcastAudio::PlayOneShot(SoundHandle sound, int32_t volume, int32_t pan)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    ActiveVoice* slot = SfxMixer.AcquireVoiceSlot(asset);
    slot->Asset = asset;
    slot->FrameOffset = 0;
    slot->Loop = false;
    slot->Active = true;
    slot->PlayId = SfxMixer.NextPlayId++;
    slot->Volume = volume;
    slot->Pan = pan;
    AudioMixer::CalculateGain(volume, pan, slot->GainLeft, slot->GainRight);
}

void DreamcastAudio::PlayLoop(SoundHandle sound, int32_t volume, int32_t pan)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    for (ActiveVoice& voice : SfxMixer.Voices)
    {
        if (voice.Active && voice.Asset == asset && voice.Loop)
        {
            voice.Volume = volume;
            voice.Pan = pan;
            AudioMixer::CalculateGain(volume, pan, voice.GainLeft, voice.GainRight);
            return;
        }
    }

    ActiveVoice* slot = SfxMixer.AcquireVoiceSlot(asset);
    slot->Asset = asset;
    slot->FrameOffset = 0;
    slot->Loop = true;
    slot->Active = true;
    slot->PlayId = SfxMixer.NextPlayId++;
    slot->Volume = volume;
    slot->Pan = pan;
    AudioMixer::CalculateGain(volume, pan, slot->GainLeft, slot->GainRight);
}

void DreamcastAudio::StopSound(SoundHandle sound)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    SfxMixer.DeactivateVoicesFor(asset);
}

void DreamcastAudio::StopCurrent(SoundHandle sound)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    ActiveVoice* newest = nullptr;
    for (ActiveVoice& voice : SfxMixer.Voices)
    {
        if (voice.Active && voice.Asset == asset)
        {
            if (newest == nullptr || voice.PlayId > newest->PlayId)
            {
                newest = &voice;
            }
        }
    }

    if (newest != nullptr)
    {
        newest->Active = false;
        newest->Asset = nullptr;
    }
}

void DreamcastAudio::SetVolume(SoundHandle sound, int32_t volume)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    for (ActiveVoice& voice : SfxMixer.Voices)
    {
        if (voice.Active && voice.Asset == asset)
        {
            voice.Volume = volume;
            AudioMixer::CalculateGain(voice.Volume, voice.Pan, voice.GainLeft, voice.GainRight);
        }
    }
}

void DreamcastAudio::SetPan(SoundHandle sound, int32_t pan)
{
    SoundAsset* asset = ToAsset(sound);
    if (asset == nullptr)
    {
        return;
    }

    printf("TODO: %s\n", __func__);

    for (ActiveVoice& voice : SfxMixer.Voices)
    {
        if (voice.Active && voice.Asset == asset)
        {
            voice.Pan = pan;
            AudioMixer::CalculateGain(voice.Volume, voice.Pan, voice.GainLeft, voice.GainRight);
        }
    }
}

void DreamcastAudio::Reset()
{
    CloseMovieStream();

    printf("TODO: %s\n", __func__);

    SfxMixer.ResetAllVoices();
}

bool DreamcastAudio::PlayMusic(int trackNumber)
{
    const MusicTrack* track = GetMusicTrack(trackNumber);
    if (track == nullptr)
    {
        return false;
    }

    if (!Init())
    {
        return false;
    }

    StopMusic();

    char relBuf[512];
    std::snprintf(relBuf, sizeof(relBuf), "mp3/%s", track->Path);
    const char* path = FullPath(relBuf);

    if (!Music.Source.Open(path))
    {
        printf("Opening MP3MusicTrack failed! %s\n", path);
        return false;
    }

    printf("TODO: %s\n", __func__);

    Music.Gain = track->Volume;
    Music.Active = true;

    return true;
}

void DreamcastAudio::StopMusic()
{
    printf("TODO: %s\n", __func__);

    CleanUpMusicStuff(Music);

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
