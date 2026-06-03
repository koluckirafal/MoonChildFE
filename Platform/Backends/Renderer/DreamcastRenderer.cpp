#include "DreamcastRenderer.h"

#include "IWindow.h"
#include "DreamcastWindow.h"

#include <algorithm>
#include <cstdio>

#include <kos.h>

static volatile int dma_done = 1;

static void dma_complete(void *userdata) {
    dma_done = 1;
    vid_flip(-1);
}

bool wait_dma(void)
{
    return !dma_done;
}

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
    pvr_dma_init();

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

void DreamcastRenderer::DrawFrame(const unsigned char*__restrict rgbaPixels, int width, int height)
{
    uint32_t vram_fb = vid_get_start(-1);

    dma_done = 0;
    pvr_dma_transfer(
        rgbaPixels,
        vram_fb,
        (width * height * 4),
        PVR_DMA_VRAM32,
        0,
        dma_complete,
        NULL
    );
}

void DreamcastRenderer::EndFrame()
{
    return;
}
