#include "DreamcastRenderer.h"

#include "IWindow.h"
#include "DreamcastWindow.h"

#include <algorithm>
#include <cstdio>

#include <kos.h>

DreamcastRenderer::DreamcastRenderer() = default;

DreamcastRenderer::~DreamcastRenderer()
{
    Destroy();
}

bool DreamcastRenderer::Init(IWindow* hostWindow)
{
    Window = hostWindow;
    DreamcastWindow* dcWindow = dynamic_cast<DreamcastWindow*>(Window);
    if (dcWindow == nullptr)
    {
        puts("DreamcastRenderer requires DreamcastWindow!");
        Window = nullptr;
        return false;
    }
    //DcWindow = dcWindow->GetNativeWindow();

    return true;
}

void DreamcastRenderer::DisplaySetVSync(bool enabled)
{
    return;
}

void DreamcastRenderer::Destroy()
{
    Window = nullptr;
    //DcWindow = nullptr;
}

void DreamcastRenderer::BeginFrame()
{
    //printf("TODO: %s\n", __func__);
}

void DreamcastRenderer::DrawFrame(const unsigned char* rgbaPixels, int width, int height)
{
    memcpy(vram_l, rgbaPixels, width * height * sizeof(vram_l[0]));
}

void DreamcastRenderer::EndFrame()
{
    vid_flip(-1);
}
