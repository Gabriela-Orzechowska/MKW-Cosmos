#include <kamek.hpp>
#include <game/Race/Kart/KartCollision.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Collision/Entity/EntityHolder.hpp>
#include <game/System/Identifiers.hpp>
#include <game/Item/ItemManager.hpp>
#include <game/Item/ItemBehaviour.hpp>
#include <game/Item/ItemSlotTable.hpp>
#include <game/Item/Obj/ItemObj.hpp>

int HandleMegaMushroomCollision(KartCollision * kartCollision)
{
    kartCollision->base.GetKartMovement()->ActivateMega();
    return -1;
}

ItemPlayer * GetItemPlayer(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = ItemManager::sInstance->players;
    u8 playerId = kartCollision->base.GetPlayerIdx();

    return &itemPlayer[playerId];
}

int HandleLightingCollision(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);
    itemPlayer->UseThunder();
    
    return -1;
}

int HandleBlooperCollision(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);
    itemPlayer->UseBlooper();
    
    return -1;
}

int HandlePowCollision(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);
    itemPlayer->UsePow();
    
    return -1;
}

int HandleBulletCollision(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);
    itemPlayer->UseBullet();
    
    return -1;
}

int HandleGoldenCollision(KartCollision * kartCollision)
{
    ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);

    if(itemPlayer->inventory.currentItemId == NONE)
    {
        itemPlayer->inventory.SetItem(GOLDEN_MUSHROOM, 0x0);
        itemPlayer->inventory.SetGoldenTimer(0x80);
        itemPlayer->inventory.SetGoldenSomething2(0xC);
    }
    itemPlayer->ActivateMushroom();
    
    return -1;
}


//Land Patches
kmWrite32(0x807A66C4, 0x60000000); 

kmWrite32(0x80796D30, 0x38600000); 
kmWrite32(0x80790EF0, 0x39800001); 
kmWrite32(0x80790EF4, 0x39600001); 
kmWrite32(0x80790EF8, 0x39400001); 
kmWrite32(0x80790EFC, 0x39200001); 


//Item Collision

//Green
//Red
//Banana
//Mushie
//Star
//BlueShell
kmWritePointer(0x808b54b8, HandleLightingCollision);
//Fib
kmWritePointer(0x808b54d0, HandleMegaMushroomCollision);
//Bob omb
kmWritePointer(0x808b54e8, HandleBlooperCollision);
kmWritePointer(0x808b54f4, HandlePowCollision);
kmWritePointer(0x808b5500, HandleGoldenCollision);
kmWritePointer(0x808b550c, HandleBulletCollision);
//Bullet
//TC

#define NEW_ITEM_LIMIT 0x10;

void IncreaseItemLimit(ItemObjProperties * src, ItemObjProperties * dst)
{
    src->limit = NEW_ITEM_LIMIT;
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

//Increase Max Item Limit
