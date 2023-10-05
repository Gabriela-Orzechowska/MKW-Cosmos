#include <kamek.hpp>
#include <game/Race/Kart/KartBase.hpp>
#include <game/Race/Kart/KartMovement.hpp>
#include <game/Visual/Effect/EffectMgr.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>

//Ported from Stebler's 200cc code

void EnableBrakeDrifting()
{
    for(int i = 0; i < RaceData::sInstance->racesScenario.localPlayerCount; i++)
    {
        u8 playerId = RaceData::sInstance->racesScenario.settings.hudPlayerIds[i];

        u32 controller = MenuData::sInstance->pad.padInfos[0].controllerSlotAndTypeActive;
        ControllerType type = ControllerType(controller & 0xFF);
        RealControllerHolder * controllerHolder = &InputData::sInstance->realControllerHolders[i];

        u16 controllerInputs = controllerHolder->inputStates[0].buttonRaw;

        u32 inputMask = 0x700;
        if(type == NUNCHUCK)
            inputMask = 0xC04;
        else if(type == GCN)
            inputMask = 0x320;
        else if(type == CLASSIC)
            inputMask = 0x250;

        if((controllerInputs & inputMask) == inputMask)
            controllerHolder->inputStates[0].buttonActions |= 0x10;

    }
}

static RaceFrameHook CheckBrakeDrift(EnableBrakeDrifting);

bool IsHoldingBrakeDrift(KartStatus * status)
{
    u32 bitfield0 = status->bitfield0;
    ControllerHolder * holder = status->base->GetControllerHolder();

    if((bitfield0 & 0x1F) != 0x0F) return false;
    if((bitfield0 & 0x80100000) != 0x00) return false;
    if((holder->inputStates[0].buttonActions & 0x10) == 0x00) return false;

    return true;
}

void BrakeDrift(KartMovement * movement)
{
    movement->UpdateKartSpeed();
    if(IsHoldingBrakeDrift(movement->base.pointers->kartStatus))
        movement->acceleration = -1.5f;
}

kmCall(0x80579910, BrakeDrift);

int EnableSound(Kart * kart, KartStatus * status)
{
    if(IsHoldingBrakeDrift(status))
        return 4;
    return status->bitfield0;
}

// Had to Check SIP with that, Credits to Melg
asm int BrakeDriftSound(){
    ASM(
        nofralloc;
        mflr r27;
        mr r30, r3;
        bl EnableSound;
        mtlr r27;
        rlwinm r0, r3, 0, 31, 31;
        rlwinm r27, r3, 31, 31, 31;
        rlwinm r28, r3, 30, 31, 31;
        mr r3, r30;
        mr r30, r0;
        blr;
    );
}

kmCall(0x806faff8, BrakeDriftSound);
