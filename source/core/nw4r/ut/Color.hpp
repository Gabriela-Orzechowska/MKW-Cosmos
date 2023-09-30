#ifndef _NW4R_UTCOLOR_
#define _NW4R_UTCOLOR_
#include "types.hpp"
#include <core/rvl/gx/GXStruct.hpp>
namespace nw4r{
namespace ut{
struct Color : public GXColor{}; // Total size 0x4
static_assert(sizeof(Color) == 0x4, "Color");
}//namespace ut
}//namespace nw4r
#endif