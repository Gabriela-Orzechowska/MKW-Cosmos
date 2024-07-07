#include "core/gamespy/gamespy.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/Scene/BaseScene.hpp>
#include <Debug/Draw/PerformanceMonitor.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Other/LicenseSettings.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRace2DMap.hpp>
#include <game/Network/RKNetController.hpp>
#include <core/rvl/DWC/DWC.hpp>

using namespace Cosmos::Data;

static u8 defaultFpsMode = 0;

void SetFrameRate(GameScene& scene, u8 mode)
{
    defaultFpsMode = mode;
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FRAME_MODE);

    if(frameMode == FORCE_30) mode = 1;
    else if(frameMode == FORCE_60) mode = 0;

    scene.SetFramerate(mode);
}

kmCall(0x80554270, SetFrameRate);
kmCall(0x80554abc, SetFrameRate);


void SetFrameRateMidGame()
{
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FRAME_MODE);
    Scene * currentScene = MenuData::GetStaticInstance()->curScene;
    if(Scene::GetType(currentScene->menuId) == CATEGORY_GAMEPLAY){
        GameScene * scene = GameScene::GetCurrent();
        if(frameMode == FRAME_MODE_DEFAULT) scene->SetFramerate(defaultFpsMode);
        else if(frameMode == FORCE_30) scene->SetFramerate(1);
        else scene->SetFramerate(0);
    }
    //Scene* currentScene = Scene::GetType(MenuData::GetStaticInstance()->curScene->menuId;

}

static SettingsValueUpdateHook svuhFramerate(SetFrameRateMidGame, Cosmos::Data::COSMOS_SETTING_FRAME_MODE);

void SetPerfMonitorVisibility()
{
    ((CosmosDebug::PerformanceMonitor *) RKSystem::mInstance.processMeter)->visible = (SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_PERFORMANCE_MONITOR) == ENABLED);
}
static SettingsUpdateHook suhSetPerfMonitorVisibility(SetPerfMonitorVisibility);
static SettingsValueUpdateHook svuhSetPerfMonitorVisibility(SetPerfMonitorVisibility, Cosmos::Data::COSMOS_SETTING_PERFORMANCE_MONITOR);
static StrapEndHook bhSetPerfMonitorVisibility(SetPerfMonitorVisibility);

extern u32 DWC_ReportLevel;
void SetDWCLogs(){
    DWC_ReportLevel = 0x0;
    if(SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_DWC_LOGS) == ENABLED) {
        DWC_ReportLevel = ~0x0;
    }
}
static SettingsUpdateHook suhDWCLogs(SetDWCLogs);
kmWrite32(0x80658be8, 0x60000000);

extern "C" {
    u64 DWC_CreateFriendKey(void * val);
}
//805eae94
void SetupLicenseButton(u32 unknown, LicenseButton& button, int licenseIndex, MiiGroup* group, int buttonIndex){
    button.SetMiiPane("mii", group, buttonIndex, 0);
    RKPD& curLicense = SaveDataManager::GetStaticInstance()->rksysRaw->licenses[licenseIndex];
    button.SetTextBoxMsg("new", 0x1781, nullptr);
    if(curLicense.magic.raw != 0x524b5044){
        button.SetPaneVisible("new", true);
        button.SetPaneVisible("mii", false);

        button.ResetTextBoxMsg("player");
    }
    else {
        button.SetPaneVisible("new", false);
        button.SetPaneVisible("mii", true);

        TextInfo miiInfo;
        miiInfo.miis[0] = group->GetMii(licenseIndex);
        button.SetTextBoxMsg("player", 0x251d, &miiInfo);

        TextInfo friendCodeInfo;
        LicenseManager& currentLicense = SaveDataManager::GetStaticInstance()->GetLicense(licenseIndex);
        u64 val = DWC_CreateFriendKey(&currentLicense.pid);
        if((val >> 32) != 0){
            friendCodeInfo.intToPass[2] = val % 10000;
            friendCodeInfo.intToPass[1] = (val / 10000) % 10000;
            friendCodeInfo.intToPass[0] = (val / 100000000) % 10000;
            button.SetTextBoxMsg("l_number", 0x83e, &friendCodeInfo);
        }
        else {
            button.SetTextBoxMsg("l_number", 0, nullptr);
        }
    }
}

void PatchLicenseButtonFile(PushButton& button, const char* folder, const char* filename, const char* variant, u32 localPlayerBitfield, u32 r8, bool inaccessible){
    button.Load(folder, "CosmosLicenseSelect", variant, localPlayerBitfield, r8, inaccessible);
}
kmBranch(0x805eae94, SetupLicenseButton);
/*
extern float MiiXthing;
extern float MiiZthing;
static float time;
void OnLicenseUpdate(Pages::LicenseSelect& page) {
    time += 0.06f;
    MiiXthing = 75.5f * sin(time);
    MiiZthing = 75.5f * cos(time);
    page.miiGroup.LoadMii(0, page.miiGroup.GetMii(0));
    for(int i = 0; i < 4; i++){
        page.licenseButtons[i].SetMiiPane("mii", &page.miiGroup, i, 0);
    }
    
}
kmWritePointer(0x808b9894, OnLicenseUpdate);
kmWrite32(0x805fa450, 0x60000000);
*/

//kmCall(0x805eacf0, PatchLicenseButtonFile);


static CtrlRace2DMap* mainMinimap = nullptr;
void SaveMinimap(CtrlRace2DMap& map, const char* name){
    map.Load(name);
    mainMinimap = &map;
}
kmCall(0x80858204, SaveMinimap);

void PatchMiiHeads() {
    if(mainMinimap == nullptr) return;
    u8 setting = SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_MII_HEADS);

    for(int i = 0; i < RaceData::GetStaticInstance()->racesScenario.GetPlayerCount(); i++){
        CtrlRace2DMapCharacter& curChar = mainMinimap->characters[i];

        bool isMii = false;
        CharacterId characterUsed = RaceData::GetStaticInstance()->racesScenario.GetPlayer(i).characterId;
        if(characterUsed >= MII_S_A_MALE){
            isMii = true;
        }

        if(setting == ENABLED){ // Mii heads enabled
            if(i == 0 || (RKNetController::GetStaticInstance()->connectionState != 0 && Cosmos::System::GetStaticInstance()->AreMiiHeadsAllowed())){
                isMii = true;
            }
        } 

        if(isMii){
            MiiGroup& group = MenuData::GetStaticInstance()->GetCurrentContext()->playerMiis;
            curChar.SetMiiPane("chara_0_0", &group, i, 5);
            curChar.SetMiiPane("chara_shadow_0_0", &group, i, 5);
            curChar.SetMiiPane("chara_shadow_0_1", &group, i, 5);
        }

        else{
            char* charTplName = CharacterIDToChar(characterUsed);
            curChar.SetPicturePane("chara_0_0", charTplName);
            curChar.SetPicturePane("chara_shadow_0_0", charTplName);
            curChar.SetPicturePane("chara_shadow_0_1", charTplName);
        }
    }
}

kmBranch(0x807eb24c, PatchMiiHeads);
static SettingsValueUpdateHook svuhMiiHeads(PatchMiiHeads, COSMOS_SETTING_MII_HEADS);

void SendOpenHostSetting() {
    GameSpy::GPConnection* connection = DWC::stpMatchCnt->connection;
    if(connection == nullptr) return;

    bool enableOpenHost = SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_OPEN_HOST) == ENABLED;
    char val[2];
    val[0] = '0' + enableOpenHost;
    val[1] = '\0';
    GameSpy::gpiSendLocalInfo(connection, "\\wwfc_openhost\\", val);
    return;
}
kmWritePointer(0x808bfe84, SendOpenHostSetting);

