#ifndef _GXTETURE_
#define _GXTETURE_

#include <types.hpp>
#include <core/rvl/gx/GXEnum.hpp>
#include <core/rvl/gx/GXStruct.hpp>

extern "C" {
    void GXInitTexObjLOD(GXTexObj *obj, GXTexFilter minFilter, GXTexFilter magFilter,
        float minLOD, float maxLOD, float lod_bias, bool biasClamp, bool doEdgeLOD, GXAnisotropy max_aniso); //801707f8
    void GXInitTexObj(
         GXTexObj*      obj,
         void*          image_ptr,
         u16            width,
         u16            height,
         GXTexFmt       format,
         GXTexWrapMode  wrap_s,
         GXTexWrapMode  wrap_t,
         bool         mipmap); //801707f8

    void GXLoadTexObj(GXTexObj* obj, GXTexMapID slot);

}
#endif