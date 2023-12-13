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
    extern u32 p_lecodeBlueDrag;
    extern u32 p_lecodeBluePmtf;
    extern u32 p_lecodeBlueFunc;
    DX::CreateBranch((u32)&p_lecodeBlueDrag, DraggableBlues);
    p_lecodeBluePmtf = (u32) &p_lecodeBlueFunc;
}
static LeCodeLoadHook ItemLeCode(LeCodeItemPatches);
kmBranch(0x807ae8ac, DraggableBlues);

// Engine Class Speed Factors

kmWrite32(0x808b5cd4, 0x3f666666); //50cc -> 0.9f => 100cc
kmWrite32(0x808b5cd8, 0x3f800000); //100cc -> 1.0f => 150cc
kmWrite32(0x808b5cdc, 0x3f8ccccd); //150cc -> 1.1f => "200"cc

// Max Speed -> 145 u/f

kmWrite32(0x808b59f4, 0x43070000);


// Team Invul
