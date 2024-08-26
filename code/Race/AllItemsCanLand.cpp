#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Race/Kart/Kart.hpp>
#include <game/Item/ItemManager.hpp>
#include <game/Item/ItemPlayer.hpp>
#include <game/Item/ItemSlotTable.hpp>

namespace Cosmos {
    inline ItemPlayer& GetItemPlayer(KartCollision* kartCollision)
    {
        ItemPlayer* itemPlayer = ItemManager::GetStaticInstance()->players;
        u8 playerId = kartCollision->base.GetPlayerIdx();

        return itemPlayer[playerId];
    }


    int HandleMegaMushroomCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer.UseMegaMushroom();
        return -1;
    }

    int HandleLightingCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer.UseThunder();
        
        return -1;
    }

    int HandleBlooperCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer.UseBlooper();
        
        return -1;
    }

    int HandlePowCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer.UsePow();
        
        return -1;
    }

    int HandleBulletCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer.UseBullet();
        
        return -1;
    }

    int HandleGoldenCollision(KartCollision * kartCollision)
    {
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return -1;
        ItemPlayer& itemPlayer = GetItemPlayer(kartCollision);

        if(itemPlayer.inventory.currentItemId == NONE)
        {
            itemPlayer.inventory.SetItem(GOLDEN_MUSHROOM, 0x0);
            itemPlayer.inventory.SetGoldenTimer(0x80);
            itemPlayer.inventory.SetGoldenSomething2(0xC);
        }
        itemPlayer.ActivateMushroom();
        
        return -1;
    }

    void EnableItemFalling(ItemObjProperties& dest, ItemObjProperties& src){
        if(!Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return;
        dest.unknown_0x58[0] = true;
        dest.unknown_0x58[1] = true;
        dest.unknown_0x58[2] = true;
        dest.unknown_0x58[3] = true;
    }
    kmBranch(0x80790fb4, EnableItemFalling);

    bool PatchTimerCheck(ItemSlotTable& holder, u32 objId, u32 r6){
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return false;
        return holder.CheckSpawnTimer(objId, r6);
    }
    kmCall(0x80796d30, PatchTimerCheck);

    bool IsItemFallingEnabled(bool ret){
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->CanAllItemsLand()) return true;
        return ret;
    }

    asm void PatchBreakCheck(){
        ASM (
                nofralloc;
                bl IsItemFallingEnabled;
                cmpwi r3, 0x0;
                blr;
            )
    }
    kmCall(0x807a66c0, PatchBreakCheck);

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
}
