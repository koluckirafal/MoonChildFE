#include "DreamcastInput.h"

#include <cstdint>
#include <cstdio>
#include <kos.h>

static constexpr int AXIS_THRESHOLD = 64;

DreamcastInput::DreamcastInput() = default;

DreamcastInput::~DreamcastInput()
{
    Destroy();
}

bool DreamcastInput::Init()
{
    maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

    if (!cont)
    {
        puts("Couldn't find any controller");
        return false;
    }

    return true;
}

void DreamcastInput::Destroy()
{
    ClearAllSources();
}

int DreamcastInput::TranslateKey(int sdlKey)
{
    return 0;
}

int DreamcastInput::TranslateGamepadButton(int button)
{
    switch (button)
    {
        case CONT_DPAD_LEFT:  return CB_LEFT;
        case CONT_DPAD_RIGHT: return CB_RIGHT;
        case CONT_DPAD_UP:    return CB_UP;
        case CONT_DPAD_DOWN:  return CB_DOWN;
        case CONT_A:          return CB_JUMP;
        case CONT_B:          return CB_ACTION;
        case CONT_X:          return CB_BACK;
        case CONT_START:      return CB_START;
        default:              return INPUT_CODE_NONE;
    }

    return INPUT_CODE_NONE;
}

void DreamcastInput::TranslateGamepadAxis(int axis, int& outNegativeCode, int& outPositiveCode)
{
    outNegativeCode = INPUT_CODE_NONE;
    outPositiveCode = INPUT_CODE_NONE;

    switch (axis)
    {
        case CONT_CAPABILITY_ANALOG_X:
            outNegativeCode = CB_LEFT;
            outPositiveCode = CB_RIGHT;
            break;
        case CONT_CAPABILITY_ANALOG_Y:
            outNegativeCode = CB_UP;
            outPositiveCode = CB_DOWN;
            break;
        default:
            break;
    }
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
    return;
}

void DreamcastInput::OnGamepadDisconnected(int instanceId)
{
    return;
}

void DreamcastInput::OnGamepadButton(int instanceId, int button, bool isDown)
{
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
