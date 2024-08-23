#include <kamek.hpp>
#include <game/Race/Kart/KartBase.hpp>
#include <game/Race/Kart/KartMovement.hpp>
#include <game/Visual/Effect/EffectMgr.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>
#include <main.hpp>

//Ported from Stebler's 200cc code

inline bool is200() {
    return RaceData::GetStaticInstance()->racesScenario.GetSettings().engineClass == CC_100;
}

void EnableBrakeDrifting()
{
    if(!is200()) return;
    for(int i = 0; i < RaceData::GetStaticInstance()->racesScenario.localPlayerCount; i++)
    {
        bool brakeDrift = false;

        u8 playerId = RaceData::GetStaticInstance()->racesScenario.GetSettings().hudPlayerIds[i];

        u32 controller = MenuData::GetStaticInstance()->pad.padInfos[i].controllerSlotAndTypeActive;
        ControllerType type = ControllerType(controller & 0xFF);
        RealControllerHolder& holder = InputData::GetStaticInstance()->GetController(i);

        using namespace CosmosController;

        switch(type) {
            case CLASSIC:
            case GCN:
                if(arePressed(holder, type, (ButtonCommon)(BUTTON_A | BUTTON_B | BUTTON_R))) brakeDrift = true;
                break;
            case NUNCHUCK:
                if(arePressed(holder, type, (ButtonCommon)(BUTTON_A | BUTTON_B | BUTTON_DPAD_DOWN))) brakeDrift = true;
                break;
            case WHEEL:
                if(arePressed(holder, type, (ButtonCommon)(BUTTON_1 | BUTTON_2 | BUTTON_B))) brakeDrift = true;
                break;
            default:
                brakeDrift = false;
        }

        if(brakeDrift) holder.inputStates[0].buttonActions |= 0x10;
    }
}

static RaceFrameHook CheckBrakeDrift(EnableBrakeDrifting);

bool IsHoldingBrakeDrift(KartStatus * status)
{
    if(!is200()) return false;

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

int BrakeDriftParticlesBike(PlayerEffects * effects)
{
    Kart * kart = effects->kart;
    if(IsHoldingBrakeDrift(kart->base.pointers->kartStatus))
        effects->DisplayEffects2(effects->bikeDriftEffects, 25, 26, true);
    else    
        effects->FadeEffects2(effects->bikeDriftEffects, 25, 26, true);

    return kart->GetDriftState();
}

kmWrite32(0x80698f88, 0x60000000);
kmCall(0x80698f8c, BrakeDriftParticlesBike);

int BrakeDriftParticlesKart(PlayerEffects& effects)
{
    Kart * kart = effects.kart;
    if(IsHoldingBrakeDrift(kart->base.pointers->kartStatus))
        effects.DisplayEffects2(effects.kartDriftEffects, 34, 36, true);
    else    
        effects.FadeEffects2(effects.kartDriftEffects, 34, 36, true);

    return kart->GetDriftState();
}

kmWrite32(0x80698048, 0x60000000);
kmCall(0x8069804c, BrakeDriftParticlesKart);
