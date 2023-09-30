#ifndef _NW4R_G3D_RESCOMMON_
#define _NW4R_G3D_RESCOMMON_
#include "types.hpp"

namespace nw4r{
namespace g3d{

struct ResBlockHeaderData{
    char kind[4];
    u32 size;
}; //0x8

template<class T>
class ResCommon{
public:
    T *data;
};
}//namespace g3d   
}//namespace nw4r


#endif