/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Input/InputData.hpp"
#include "System/Identifiers.hpp"
#include <kamek.hpp>
#include <game/Race/Kart/KartBase.hpp>
#include <game/Race/Kart/KartMovement.hpp>
#include <game/Visual/Effect/EffectMgr.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>
#include <System/System.hpp>

//Ported from Stebler's 200cc code

inline bool is200() {
    return RaceData::GetStaticInstance()->racesScenario.GetSettings().engineClass == CC_100;
}

void EnableBrakeDrifting(ControllerHolder* holder){

        bool brakeDrift = false;

        ControllerId type = (ControllerId) holder->GetCurrentController()->GetType();

        using namespace CosmosController;

        switch(type) {
            case CONTROLLER_CLASSIC:
            case CONTROLLER_GCN:
                if(arePressed(*holder, type, (ButtonCommon)(BUTTON_A | BUTTON_B | BUTTON_R))) brakeDrift = true;
                break;
            case CONTROLLER_NUNCHUCK:
                if(arePressed(*holder, type, (ButtonCommon)(BUTTON_A | BUTTON_B | BUTTON_DPAD_DOWN))) brakeDrift = true;
                break;
            case CONTROLLER_WII_WHEEL:
                if(arePressed(*holder, type, (ButtonCommon)(BUTTON_1 | BUTTON_2 | BUTTON_B))) brakeDrift = true;
                break;
            default:
                brakeDrift = false;
        }

        if(brakeDrift) holder->inputStates[0].buttonActions |= 0x10;
}

void EnableBrakeDrifting()
{
    if(!is200()) return;
    MenuDataPad* pad = &MenuData::GetStaticInstance()->pad;
    for(int i = 0; i < RaceData::GetStaticInstance()->racesScenario.localPlayerCount; i++)
    {
        ControllerHolder* holder = pad->GetControllerHolder(i);
        if(holder) EnableBrakeDrifting(holder);
    }
}

static RaceFrameHook CheckBrakeDrift(EnableBrakeDrifting);

void FixGhosts(GhostWriter* writer, u16 buttonActions, u8 quantisedStickX,
    u8 quantisedStickY, u8 motionControlFlickUnmirrored) {
    register ControllerHolder* controllerHolder;
    asm{ASM (mr controllerHolder, r30;)};
    EnableBrakeDrifting(controllerHolder);
    writer->WriteFrame(controllerHolder->inputStates[0].buttonActions & ~0x20, quantisedStickX, quantisedStickY, motionControlFlickUnmirrored);
}
kmCall(0x80521828, FixGhosts);

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
