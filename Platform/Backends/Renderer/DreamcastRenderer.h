#pragma once

#include "IRenderer.h"

#include <kos.h>

class DreamcastRenderer final : public IRenderer
{
public:
    DreamcastRenderer();
    ~DreamcastRenderer() override;

    bool Init(IWindow* hostWindow) override;
    void Destroy() override;

    void BeginFrame() override;
    void DrawFrame(const unsigned char* rgbaPixels, int width, int height) override;
    void EndFrame() override;

    void DisplaySetVSync(bool enabled) override;

private:
    IWindow* Window = nullptr;
};
