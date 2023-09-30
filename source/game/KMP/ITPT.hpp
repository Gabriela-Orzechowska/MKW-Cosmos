#ifndef _ITPT_
#define _ITPT_
#include <game/KMP/SectionBase.hpp>

struct ITPT{ 
    Vec3 position;
    float width;
    u16 setting1;
    u16 setting2;
}; //total size 0x14

typedef struct Section<ITPT> ITPTSection;


#endif