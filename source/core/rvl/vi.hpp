#ifndef _VI_
#define _VI_
#include "types.hpp"


extern "C"{
    void *VIGetNextFrameBuffer(); //801bab24
    void VIWaitForRetrace(); //801b99ec
    void VIConfigure(u32 param_1); //801bab2c
    void VIFlush(); //801ba9a4
    u32 VIGetCurrentLine(); //801bac48
    void VISetBlack(u32 param_1); //801bab2c
}
#endif