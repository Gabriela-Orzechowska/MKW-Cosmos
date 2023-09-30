#ifndef _ITEMOBJPOW_
#define _ITEMOBJPOW_
#include <kamek.hpp>
#include <game/Item/Obj/ItemObj.hpp>
#include <game/Visual/Model/ModelDirector.hpp>

//_sinit_ at 807b2ed0
class ItemObjPow : public ItemObj{
public:
    static ItemObj** CreateArray(u32 count); //807b1ab0
    void InitSelf() override; //0x8 807b1ba4 vtable 808d2530
};
static_assert(sizeof(ItemObjPow) == 0x1a0, "ItemObjPow");

class PowPage{ //one per Page
    PowPage(u32 pageId); //807b2540
    ModelDirector *powModel; //0x0
    u32 pageId; //0x4
    u8 unknown_0x8[0x10-0x8];
}; //0x10
static_assert(sizeof(PowPage) == 0x10, "PowPage");

class PowMgr {
public:
    static PowMgr *sInstance; //809c3660
    static PowMgr *GetStaticInstance(); //807b1bc0
    static void DestroyStaticInstance(); //807b1c70
    PowMgr(); //807b1d18 inlined
    void DeployBlooper(u8 playerId); //807a9128 that player used a blooper which will deploy the blooper model from the character
    u32 unknown_0x0;
    u32 pageCount; //0x4
    u8 unknown_0x8[0x18-0x8];
    PowPage *powPages[4]; //0x18
};//Total Size 0x54
static_assert(sizeof(PowMgr) == 0x28, "PowMgr");



#endif