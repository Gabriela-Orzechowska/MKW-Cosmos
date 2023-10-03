#include <kamek.hpp>
#include <game/Race/Kart/KartCollision.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Collision/Entity/EntityHolder.hpp>
#include <game/System/Identifiers.hpp>
#include <game/Item/ItemManager.hpp>
#include <game/Item/ItemBehaviour.hpp>
#include <game/Item/ItemSlotTable.hpp>
#include <game/Item/Obj/ItemObj.hpp>

//Increase Item Limit

#define NEW_ITEM_LIMIT 0x10;

void IncreaseItemLimit(ItemObjProperties * src, ItemObjProperties * dst)
{
    CopyItemOBJPropertiesFromRelToTable(src, dst);
    dst->limit = NEW_ITEM_LIMIT;
    return;
}

kmCall(0x80790b74, IncreaseItemLimit); //BlueShell
kmCall(0x80790b84, IncreaseItemLimit); //Lighting
kmCall(0x80790ba4, IncreaseItemLimit); //Mega
kmCall(0x80790bd4, IncreaseItemLimit); //Pow
kmCall(0x80790be4, IncreaseItemLimit); //Golden
kmCall(0x80790bf4, IncreaseItemLimit); //Bullet
kmCall(0x80790bc4, IncreaseItemLimit); //Blooper

// No Bill Icon
kmWrite32(0x807a9b28, 0x60000000);
