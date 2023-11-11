#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <core/nw4r/g3d/Camera.hpp>
#include <core/egg/Display.hpp>
#include <core/System/RKSystem.hpp>

void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color)
{
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GX_Position2s16(x,y);
        GX_Position2s16(x + width,y);
        GX_Position2s16(x + width,y + height);
        GX_Position2s16(x,y + height);
    }

    GXEnd();

}

void AsyncDisplay_endRender(EGG::AsyncDisplay * display);

void RenderTest(EGG::AsyncDisplay * display)
{
    GXSetViewport(0.0f,0.0f,608.0f,456.0f,0.0f,1.0f);
    GXSetScissor(0,0, 608, 456);
    float mtx[4][4];
    C_MTXOrtho(mtx, 0.0f, 456.0f, 0.0f, 608.0f, 0.0f, -1.0f);
    GXSetProjection(mtx, GX_ORTHOGRAPHIC);
    float posMtx[3][4];
    PSMTXIdentity(posMtx);
    GXLoadPosMtxImm(posMtx,0);
    GXSetCurrentMtx(0);

    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumChans(1);
    GXSetNumIndStages(0);
    GXSetNumTexGens(0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetNumTevStages(1);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVDSTALPHA, GX_LO_COPY);
    GXSetColorUpdate(GX_TRUE);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);

    AsyncDisplay_endRender(display);
    return;
}

kmWritePointer(0x802a26f0, RenderTest);