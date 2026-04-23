#pragma once

#include "IInput.h"

#include <kos.h>

#include <cstdint>
#include <deque>

class DreamcastInput final : public IInput
{
public:
    DreamcastInput();
    ~DreamcastInput() override;

    bool Init() override;
    void Destroy() override;

    void OnKeyEvent(int nativeKeyCode, bool isDown, bool isRepeat) override;
    void OnGamepadConnected(int instanceId) override;
    void OnGamepadDisconnected(int instanceId) override;
    void OnGamepadButton(int instanceId, int button, bool isDown) override;
    void OnGamepadAxis(int instanceId, int axis, int value) override;
    void OnFocusLost() override;

    bool PollNext(InputEvent& out) override;

private:
    static int TranslateKey(int sdlKey);
    static int TranslateGamepadButton(int button);
    static void TranslateGamepadAxis(int axis, int& outNegativeCode, int& outPositiveCode);

    void SetSource(uint32_t sourceId, int code, bool isDown);
    void ClearAllSources();

    std::deque<InputEvent> Queue;
};
