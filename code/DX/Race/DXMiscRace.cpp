#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Item/ItemManager.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>

#ifndef WORLDWIDE_TEST

namespace DX
{
    void MegaTC(KartMovement * kartMovement)
    {
        kartMovement->ActivateMega();
    }
    kmCall(0x80580630, MegaTC);

    void MegaFov(KartMovement * kartMovement)
    {
        kartMovement->UpdatePow();
        RaceCamera * camera = kartMovement->base.pointers->raceCamera;
        if(camera != NULL)
        {
            if(kartMovement->base.pointers->kartStatus->bitfield2 & BITFIELD2::IN_MEGA)
                camera->additionalFov = 30.0;
        }
    }
    kmCall(0x805795d8, MegaFov); 
    kmWrite32(0x805a252c, 0x70600500);

    // Engine Class Speed Factors

    kmWrite32(0x808b5cd4, 0x3f666666); //50cc -> 0.9f => 100cc
    kmWrite32(0x808b5cd8, 0x3f800000); //100cc -> 1.0f => 150cc
    kmWrite32(0x808b5cdc, 0x3f8ccccd); //150cc -> 1.1f => "200"cc

    // Max Speed -> 145 u/f
    kmWrite32(0x808b59f4, 0x43070000);
    
} // namespace DX


#endif