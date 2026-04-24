#include "DreamcastInput.h"

#include <cstdio>

static constexpr int AXIS_THRESHOLD = 16000;

DreamcastInput::DreamcastInput() = default;

DreamcastInput::~DreamcastInput()
{
    Destroy();
}

bool DreamcastInput::Init()
{
    printf("TODO: %s\n", __func__);

    return true;
}

void DreamcastInput::Destroy()
{
    printf("TODO: %s\n", __func__);

    ClearAllSources();
}

int DreamcastInput::TranslateKey(int sdlKey)
{
    printf("TODO: %s\n", __func__);

    return 0;
}

int DreamcastInput::TranslateGamepadButton(int button)
{
    printf("TODO: %s\n", __func__);

    return INPUT_CODE_NONE;
}

void DreamcastInput::TranslateGamepadAxis(int axis, int& outNegativeCode, int& outPositiveCode)
{
    outNegativeCode = INPUT_CODE_NONE;
    outPositiveCode = INPUT_CODE_NONE;

    printf("TODO: %s\n", __func__);
}

void DreamcastInput::SetSource(uint32_t sourceId, int code, bool isDown)
{
    if (isDown && code == INPUT_CODE_NONE)
    {
        return;
    }

    Queue.push_back({sourceId, code, isDown});
}

void DreamcastInput::ClearAllSources()
{
    Queue.push_back({0, 0, false});
}

void DreamcastInput::OnKeyEvent(int nativeKeyCode, bool isDown, bool isRepeat)
{
    if (isRepeat)
    {
        return;
    }
    const int code = TranslateKey(nativeKeyCode);
    if (code == 0)
    {
        return;
    }
    const uint32_t sourceId = INPUT_SOURCE_KEY | static_cast<uint32_t>(nativeKeyCode & INPUT_SOURCE_CODE_MASK);
    SetSource(sourceId, code, isDown);
}

void DreamcastInput::OnGamepadConnected(int instanceId)
{
    printf("TODO: %s\n", __func__);
}

void DreamcastInput::OnGamepadDisconnected(int instanceId)
{
    printf("TODO: %s\n", __func__);
}

void DreamcastInput::OnGamepadButton(int instanceId, int button, bool isDown)
{
    printf("TODO: %s\n", __func__);

    const int code = TranslateGamepadButton(button);
    if (code == 0)
    {
        return;
    }

    const uint32_t sourceId = INPUT_SOURCE_GAMEPAD_BUTTON | static_cast<uint32_t>(button & INPUT_SOURCE_CODE_MASK);
    SetSource(sourceId, code, isDown);
}

void DreamcastInput::OnGamepadAxis(int instanceId, int axis, int value)
{
    printf("TODO: %s\n", __func__);

    int negativeCode = INPUT_CODE_NONE;
    int positiveCode = INPUT_CODE_NONE;

    TranslateGamepadAxis(axis, negativeCode, positiveCode);
    if (negativeCode == INPUT_CODE_NONE && positiveCode == INPUT_CODE_NONE)
    {
        return;
    }

    const uint32_t negSourceId = INPUT_SOURCE_GAMEPAD_AXIS_NEG | static_cast<uint32_t>(axis & INPUT_SOURCE_CODE_MASK);
    const uint32_t posSourceId = INPUT_SOURCE_GAMEPAD_AXIS_POS | static_cast<uint32_t>(axis & INPUT_SOURCE_CODE_MASK);

    SetSource(negSourceId, negativeCode, value <= -AXIS_THRESHOLD);
    SetSource(posSourceId, positiveCode, value >=  AXIS_THRESHOLD);
}

void DreamcastInput::OnFocusLost()
{
    ClearAllSources();
}

bool DreamcastInput::PollNext(InputEvent& out)
{
    if (Queue.empty())
    {
        return false;
    }
    out = Queue.front();
    Queue.pop_front();
    return true;
}
