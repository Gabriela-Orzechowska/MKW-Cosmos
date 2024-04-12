#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/Item/ItemManager.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>
#include <game/KMP/STGI.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRace2DMap.hpp>

void DraggableBlues(ItemPlayerSub *sub)
{
    using namespace CosmosData;
    if(SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_DRAGGABLE_BLUES) == DISABLED) sub->isNotDragged = true;
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

// Lap Modifier

RaceinfoPlayer * LoadLapCountFromKMP(RaceinfoPlayer * info, u8 index)
{
    u8 lapCount = KMP::Controller::sInstance->stageInfo->pointArray[0]->raw->lapCount;
    RaceData::sInstance->racesScenario.settings.lapCount = lapCount;
    return new(info) RaceinfoPlayer(index, lapCount);
}


kmCall(0x805328d4, LoadLapCountFromKMP);

kmWrite32(0x805336B8, 0x60000000);
kmWrite32(0x80534350, 0x60000000);
kmWrite32(0x80534BBC, 0x60000000);
kmWrite32(0x80723D10, 0x281D0009);
kmWrite32(0x80723D40, 0x3BA00009);


// Secondary KTPT
KMP::KTPTHolder * GetLineKTPT(const KMP::Controller * controller, u32 idx)
{
    KMP::KTPTHolder * holder = controller->GetKTPTHolder(1);
    if(!holder) holder = controller->GetKTPTHolder(0);
    return holder;
}

kmCall(0x807ea670, GetLineKTPT);

void CorrectKTPTRotationMirror(CtrlRace2DMapObject * object, Vec3 * rotation)
{
    if(RaceData::sInstance->racesScenario.settings.modeFlags & 1){
        rotation->y *= -1;
    }

    object->SetMapRotation(rotation);
}

kmCall(0x807ea6e0, CorrectKTPTRotationMirror);
