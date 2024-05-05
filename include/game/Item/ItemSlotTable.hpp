#ifndef _ITEMSLOTTABLE_
#define _ITEMSLOTTABLE_
#include <kamek.hpp>
#include <core/egg/mem/Disposer.hpp>
#include <game/Item/Obj/ItemObj.hpp>
#include <game/Item/ItemPlayer.hpp>
#include <game/Race/Kart/KartPointers.hpp>
#include <game/KMP/Controller.hpp>
#include <game/System/Identifiers.hpp>

struct ItemSlotTableHolder{
    u32 columns;
    s32 * data;
};

class ItemSlotTable {
public:
    static ItemSlotTable * sInstance; //809c3670
    static void DestroyInstance(); //807ba814
    ItemSlotTable(); //807ba8c8
    ~ItemSlotTable(); //807ba650
    void ResetLightingTimer(); //807bb9bc
    EGG::TDisposer<ItemSlotTable> disposer; //0x0
    ItemSlotTableHolder playerChances; //0x10
    ItemSlotTableHolder cpuChances; //0x18
    ItemSlotTableHolder specialChaces; //0x20
    u32 * itemsInWheel; //0x28
    u32 * specialBoxItemsInWheel; //0x2c
    void * specialChances; //0x30
    s32 lightingTimer; //0x34
    s32 blueShellTimer; //0x38
    s32 blooperTimer; //0x3c
    s32 powTimer; //0x40
    u32 playerCount; //0x44
};
size_assert(ItemSlotTable, 0x48);

#endif