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
    
    pvr_init_params_t params = {
        {
            PVR_BINSIZE_16,
            0,
            0,
            0,
            0
        },
        1024 * 512
    };

    if (pvr_init(&params) < 0)
    {
        puts("PVR init failed");
        return false;
    }

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
    pvr_wait_ready();
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_OP_POLY);
}

void DreamcastRenderer::DrawFrame(const unsigned char* rgbaPixels, int width, int height)
{
	pvr_ptr_t texture = reinterpret_cast<pvr_ptr_t>(const_cast<unsigned char*>(rgbaPixels));

    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;

    pvr_poly_cxt_txr(
        &cxt,
        PVR_LIST_OP_POLY,

        PVR_TXRFMT_PAL8BPP |
        PVR_TXRFMT_TWIDDLED,

        1024,
        512,

        texture,

        PVR_FILTER_NONE
    );

    pvr_poly_compile(&hdr, &cxt);

    pvr_prim(&hdr, sizeof(hdr));

    float u2 = 640.0f / 1024.0f;
    float v2 = 480.0f / 512.0f;

    pvr_vertex_t v;
    memset(&v, 0, sizeof(v));

    /* TL */
    v.flags = PVR_CMD_VERTEX;
    v.x = 0.f;
    v.y = 0.f;
    v.z = 1.f;
    v.u = 0.f;
    v.v = 0.f;
    v.argb = 0xffffffff;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    /* TR */
    v.flags = PVR_CMD_VERTEX;
    v.x = 640.f;
    v.y = 0.f;
    v.z = 1.f;
    v.u = u2;
    v.v = 0.f;
    v.argb = 0xffffffff;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    /* BL */
    v.flags = PVR_CMD_VERTEX;
    v.x = 0.f;
    v.y = 480.f;
    v.z = 1.f;
    v.u = 0.f;
    v.v = v2;
    v.argb = 0xffffffff;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));

    /* BR */
    v.flags = PVR_CMD_VERTEX_EOL;
    v.x = 640.f;
    v.y = 480.f;
    v.z = 1.f;
    v.u = u2;
    v.v = v2;
    v.argb = 0xffffffff;
    v.oargb = 0;
    pvr_prim(&v, sizeof(v));
}

void DreamcastRenderer::EndFrame()
{
    pvr_list_finish();
    pvr_scene_finish();
}
