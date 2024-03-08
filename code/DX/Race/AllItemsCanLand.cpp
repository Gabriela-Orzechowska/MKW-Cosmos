#include <kamek.hpp>
#include <game/Race/Kart/KartCollision.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Collision/Entity/EntityHolder.hpp>
#include <game/System/Identifiers.hpp>
#include <game/Item/ItemManager.hpp>
#include <game/Item/ItemBehaviour.hpp>

namespace DX
{
    ItemPlayer * GetItemPlayer(KartCollision * kartCollision)
    {
        ItemPlayer * itemPlayer = ItemManager::sInstance->players;
        u8 playerId = kartCollision->base.GetPlayerIdx();

        return &itemPlayer[playerId];
    }

    #pragma region AllItemsCanLand

    int HandleMegaMushroomCollision(KartCollision * kartCollision)
    {
        ItemPlayer * itemPlayer = GetItemPlayer(kartCollision);
        itemPlayer->UseMegaMushroom();
        return -1;
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

} // namespace DX
