#ifndef _GX_
#define _GX_

#include "types.hpp"
#include <core/rvl/gx/GXEnum.hpp>
#include <core/rvl/gx/GXStruct.hpp>

extern "C" {
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GXClearVtxDesc();
void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac);
void GXSetNumTexGens(u8 nr);
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts);
void GXSetCullMode(GXCullMode mode);
void GXSetNumChans(u8 nChans);
void GXSetChanCtrl(GXChannelID chan, bool enable, GXColorSrc amb_src, GXColorSrc mat_src, u32 light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn);
void GXSetTevOp(GXTevStageID id, GXTevMode mode);
void GXSetAlphaCompare(GXCompare comp0, u8 ref0, GXAlphaOp op, GXCompare comp1, u8 ref1);
void GXSetAlphaUpdate(bool enable);
void GXSetColorUpdate(bool enable);
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID IDcolor);
void GXSetNumTevStages(u8 num);
void GXSetFog(GXFogType type, float startz, float endz, float nearz, float farz, GXColor color);
void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op);
void GXSetZMode(bool compare_enable, GXCompare func, bool update_enable);
void GXSetZCompLoc(bool before_tex);
void GXSetProjection(const float mtx[4][4], GXProjectionType type);
void GXLoadPosMtxImm(const float mtx[3][4], u32 pnidx);
void GXSetCurrentMtx(u32 mtx);
void GXLoadPosMtxIndx(u32 indx);
void GXSetViewport(float x, float y, float width, float height, float near, float far);
void GXSetScissor(u32 left, u32 top, u32 right, u32 bottom);
void GXSetScissorBoxOffset(u32 x, u32 y);
void GXSetNumIndStages(u32 i);
void GXSetChanMatColor(GXChannelID id, GXColor color);


}
#endif