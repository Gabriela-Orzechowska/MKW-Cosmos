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

void MegaTC(KartMovement& movement){
    if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsMegaCloudEnabled()) movement.ActivateMega();
    else movement.ApplyLightningEffect(0x264, 0, 1);
}
kmCall(0x80580630, MegaTC);


