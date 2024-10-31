/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
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

#include "System/identifiers.hpp"
#include "UI/Layout/ControlLoader.hpp"
#include "core/nw4r/g3d/ScnMdl.hpp"
#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Item/ItemManager.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>
#include <game/KMP/STGI.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRace2DMap.hpp>
#include <game/Item/Obj/ItemObj.hpp>
#include <game/Item/Obj/Kumo.hpp>

void DraggableBlues(ItemPlayerSub& sub)
{
    using namespace Cosmos::Data;
    if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_DRAGGABLE_BLUES) == DISABLED) sub.isNotDragged = true;
}

kmBranch(0x807ae8ac, DraggableBlues);

// No Team Invincibility (From Melg)
kmWrite32(0x8056fd24, 0x38000000); //KartCollision::CheckKartCollision()
kmWrite32(0x80572618, 0x38000000); //KartCollision::CheckItemCollision()
kmWrite32(0x80573290, 0x38000000); //KartCollision::HandleFIBCollision()
kmWrite32(0x8068e2d0, 0x38000000); //PlayerEffects ctor
kmWrite32(0x8068e314, 0x38000000); //PlayerEffects ctor
kmWrite32(0x807a7f6c, 0x38c00000); //FIB are always red
kmWrite32(0x807b0bd4, 0x38000000); //pass TC to teammate
kmWrite32(0x807bd2bc, 0x38000000); //RaceGlobals
kmWrite32(0x807f18c8, 0x38000000); //TC alert


kmWrite32(0x808b5cd8, 0x3f800000); //100cc -> 1.0f for 200
kmWrite32(0x808b5cd4, 0x3f666666); //50cc -> 0.9f for 100 

// Lap Modifier

RaceinfoPlayer * LoadLapCountFromKMP(RaceinfoPlayer * info, u8 index)
{
    u8 lapCount = KMP::Controller::GetStaticInstance()->stageInfo->GetPoint(0)->raw->lapCount;
    RaceData::GetStaticInstance()->racesScenario.GetSettings().lapCount = lapCount;
    return new(info) RaceinfoPlayer(index, lapCount);
}


kmCall(0x805328d4, LoadLapCountFromKMP);

kmWrite32(0x805336B8, 0x60000000);
kmWrite32(0x80534350, 0x60000000);
kmWrite32(0x80534BBC, 0x60000000);
kmWrite32(0x80723D10, 0x281D0009);
kmWrite32(0x80723D40, 0x3BA00009);


// Secondary KTPT
KMP::KTPTHolder* GetLineKTPT(const KMP::Controller& controller)
{
    KMP::KTPTHolder* holder = controller.GetKTPTHolder(1);
    if(!holder) holder = controller.GetKTPTHolder(0);
    return holder;
}
kmCall(0x807ea670, GetLineKTPT);

void CorrectKTPTRotationMirror(CtrlRace2DMapObject& object, Vec3* rotation)
{
    if(RaceData::GetStaticInstance()->racesScenario.GetSettings().isMirror()){
        rotation->y *= -1;
    }
    object.SetMapRotation(rotation);
}
kmCall(0x807ea6e0, CorrectKTPTRotationMirror);

// Speedmod

void SetSpeedmod(Kart& kart){

    bool is200 = RaceData::GetStaticInstance()->racesScenario.settings.engineClass == CC_100;
    bool is100 = RaceData::GetStaticInstance()->racesScenario.settings.engineClass == CC_50;

    float factor = 1.0f;
    if(is200) factor = 1.5f;

    u32 mod = KMP::Controller::GetStaticInstance()->stageInfo->pointArray[0]->raw->speedMod;
    mod = mod << 16;
    float speedMod = *(float*)&mod;

    if(speedMod == 0.0f) speedMod = 1.0f;
    factor *= speedMod;

    greenShellSpeed = 105.f * factor;
    redShellInitialSpeed = 75.0f * factor;
    redShellSpeed = 130.0f * factor;
    blueShellHomingSpeed = 130.0f * factor;
    blueShellMinimumDiveDistance = 6400000.0f * factor;
    blueShellSpeed = 260.0f * factor;

    hardSpeedCap = 120.0f * factor;
    bulletSpeed = 145.0f * factor;
    starSpeed = 105.0f * factor;
    megaTCSpeed = 95.0f * factor;

    KartStats* stats = kart.values->statsAndBsp->stats;

    stats->baseSpeed *= factor;
    stats->standard_acceleration_as[0] *= factor;
    stats->standard_acceleration_as[1] *= factor;
    stats->standard_acceleration_as[2] *= factor;
    stats->standard_acceleration_as[3] *= factor;

    minDriftSpeedRatio = is200 ? 0.36f : 0.55f;
    regularBoostAccel = 3.0f * factor;
    unknown_70 = 70.0f * factor;

    kart.Init();
}
kmCall(0x8058f778, SetSpeedmod);

void LoadNewItemWindow(ControlLoader& loader, const char* folder, const char* name, 
        const char* variant, const char** animations){
    loader.Load(folder, "CosmosItemWindow", variant, animations);
}
kmCall(0x807ef50c, LoadNewItemWindow);

void LoadNewChaseIcon(ControlLoader& loader, const char* folder, const char* name, 
        const char* variant, const char** animations){
    loader.Load(folder, "CosmosChaseIcon", variant, animations);
}
kmCall(0x807f2064, LoadNewChaseIcon);
void MegaTC(KartMovement& movement){
    if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsMegaCloudEnabled()) movement.ActivateMega();
    else movement.ApplyLightningEffect(0x264, 0, 1);
}
kmCall(0x80580630, MegaTC);

void LoadMegaModel(ItemObjKumo& obj, const char* name, const char* shadowName, u32 r6, ItemObjBase::AnmParam* r7){
    if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsMegaCloudEnabled()) {
        obj.LoadGraphics("kumo_mega.brres", name, shadowName, r6, r7, nw4r::g3d::ScnMdl::BUFFER_NONE, nullptr, 0);
    }
    else obj.LoadGraphicsImplicitBRRES(name, shadowName, r6, r7, nw4r::g3d::ScnMdl::BUFFER_NONE, nullptr);
}
kmCall(0x807af568, LoadMegaModel); 

extern char* ItemPaneNameTable[1]; //808af88c

char* GetItemPane(ItemId itemType, u32 itemCount){
    if(itemType == TRIPLE_MUSHROOM){
        switch(itemCount){
            case 1:
                return "kinoko";
            case 2:
                return "kinoko_2";
            default:
                return "kinoko_3";
        }
    }
    else if(itemType == THUNDER_CLOUD && 
            Cosmos::Data::SettingsHolder::GetStaticInstance()->IsMegaCloudEnabled()){
        return "thunder_mega";
    }
    return ItemPaneNameTable[itemType];
}
kmBranch(0x80860af0, GetItemPane);


