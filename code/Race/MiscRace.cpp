#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Item/ItemManager.hpp>

void MegaTC(KartMovement * kartMovement)
{
    kartMovement->ActivateMega();
}
kmCall(0x80580630, MegaTC);

void MegaFov(KartMovement * kartMovement)
{
    kartMovement->UpdatePow();
    UnkCameraStruct * camera = kartMovement->base.pointers->camera_0x24;
    if(camera != NULL)
    {
        if(kartMovement->base.pointers->kartStatus->bitfield2 & 0x8000)
        {
            camera->targetVFov = 30.0;
        }
    }
   
}
kmCall(0x805795d8, MegaFov); 