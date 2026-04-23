#pragma once

#include "IWindow.h"

#include <kos.h>

#define WIDTH 640
#define HEIGHT 480

class DreamcastWindow final : public IWindow
{
public:
    DreamcastWindow();
    ~DreamcastWindow() override;

    bool Create(const char* title, int width, int height) override;
    void Destroy() override;

    WindowSize GetPixelSize() const override;

    void DisplaySetFullscreen(bool enabled) override;

    void PumpOSEvents(IInput* sink, bool& outExitRequested) override;
};
