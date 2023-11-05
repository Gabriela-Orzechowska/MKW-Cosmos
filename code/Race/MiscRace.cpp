#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Item/ItemManager.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>

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

    u32 controller = MenuData::sInstance->pad.padInfos[0].controllerSlotAndTypeActive;
    ControllerType type = ControllerType(controller & 0xFF);
    RealControllerHolder * holder = &InputData::sInstance->realControllerHolders[0];

    if(DXController::isPressed(holder,type,DXController::BUTTON_Y, true))
    {
        if(camera->flags & 0x8)
        {
            camera->flags &= ~0x18;
            camera->enableFovEffects = 1;
        }
        else
        {
            camera->flags |= 0x18;
            camera->enableFovEffects = 0;
        }
    }

}
kmCall(0x805795d8, MegaFov); 
kmWrite32(0x805a252c, 0x70600500);

void DraggableBlues(ItemPlayerSub *sub)
{
    using namespace DXData;
    if(SettingsHolder::GetInstance()->GetSettings()->pages[DX_RACE_SETTINGS_1].setting[DX_DRAGGABLE_BLUES] == DISABLED) sub->isNotDragged = true;
}

void LeCodeItemPatches()
{
    DX::CreateBranch(DX::GetPortAddress(0x807ae8ac, 0x807a3c04, 0x807adf18, 0x8079cc6c), DraggableBlues);
    *(u32 *)DX::GetPortAddress(0x808a5b30, 0x808a1058, 0x808a4c90, 0x80893f90) = DX::GetPortAddress(0x807ae8a8, 0x807a3c00, 0x807adf14, 0x8079cc68);
}
static LeCodeLoadHook ItemLeCode(LeCodeItemPatches);
kmBranch(0x807ae8ac, DraggableBlues);

// Engine Class Speed Factors

kmWrite32(0x808b5cd4, 0x3f666666); //50cc -> 0.9f => 100cc
kmWrite32(0x808b5cd8, 0x3f800000); //100cc -> 1.0f => 150cc
kmWrite32(0x808b5cdc, 0x3f8ccccd); //150cc -> 1.1f => "200"cc

// Max Speed -> 135 u/f

kmWrite32(0x808b59f4, 0x43070000);


// Team Invul
