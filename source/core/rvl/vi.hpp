#ifndef _VI_
#define _VI_
#include "types.hpp"


extern "C"{
    void *VIGetNextFrameBuffer(); //801bab24
    void *VIWaitForRetrace(); //801b99ec
}
#endif