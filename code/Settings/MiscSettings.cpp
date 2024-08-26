#include "System/identifiers.hpp"
#include "UI/MenuData/MenuData.hpp"
#include "UI/Page/Menu/VSModeSelect.hpp"
#include "UI/Settings/NewSettingsPage.hpp"
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
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_FRAME_MODE);

    if(frameMode == FORCE_30) mode = 1;
    else if(frameMode == FORCE_60) mode = 0;

    scene.SetFramerate(mode);
}

kmCall(0x80554270, SetFrameRate);
kmCall(0x80554abc, SetFrameRate);


void SetFrameRateMidGame()
{
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_FRAME_MODE);
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
    ((CosmosDebug::PerformanceMonitor *) RKSystem::mInstance.processMeter)->visible = (SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_PERFORMANCE_MONITOR) == ENABLED);
}
static SettingsUpdateHook suhSetPerfMonitorVisibility(SetPerfMonitorVisibility);
static SettingsValueUpdateHook svuhSetPerfMonitorVisibility(SetPerfMonitorVisibility, Cosmos::Data::COSMOS_SETTING_PERFORMANCE_MONITOR);
static StrapEndHook bhSetPerfMonitorVisibility(SetPerfMonitorVisibility);

extern u32 DWC_ReportLevel;
void SetDWCLogs(){
    DWC_ReportLevel = 0x0;
    if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_DWC_LOGS) == ENABLED) {
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

static CtrlRace2DMap* mainMinimap = nullptr;
void SaveMinimap(CtrlRace2DMap& map, const char* name){
    map.Load(name);
    mainMinimap = &map;
}
kmCall(0x80858204, SaveMinimap);

void PatchMiiHeads() {
    if(mainMinimap == nullptr) return;
    u8 setting = SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_MII_HEADS);

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

    bool enableOpenHost = SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_OPEN_HOST) == ENABLED;
    char val[2];
    val[0] = '0' + enableOpenHost;
    val[1] = '\0';
    GameSpy::gpiSendLocalInfo(connection, "\\wwfc_openhost\\", val);
    return;
}
kmWritePointer(0x808bfe84, SendOpenHostSetting);

void SetLicenseSettings(LicenseManager& license, LicenseManager::Modes mode)
{
    using namespace Cosmos::Data;
    SettingsHolder* holder = SettingsHolder::GetStaticInstance();
    license.settings[0][mode] = holder->GetSettingValue(COSMOS_SETTING_VS_CLASS);
    license.settings[1][mode] = holder->GetSettingValue(COSMOS_SETTING_VS_CPU);
    license.settings[2][mode] = holder->GetSettingValue(COSMOS_SETTING_VS_VEHICLES);
    license.settings[3][mode] = holder->GetSettingValue(COSMOS_SETTING_VS_COURSES);
    license.settings[4][mode] = holder->GetSettingValue(COSMOS_SETTING_VS_ITEMS);
}

const u8 raceCounts[8] = {2, 4, 8, 12, 16, 32};
void ApplyVSSettings(){
    LicenseManager& license = SaveDataManager::GetStaticInstance()->GetCurrentLicense();
    Cosmos::Data::SettingsHolder* holder = Cosmos::Data::SettingsHolder::GetStaticInstance();

    SetLicenseSettings(license, LicenseManager::VS_SINGLE);
    SetLicenseSettings(license, LicenseManager::VS_MULTI);

    GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
    RacedataScenario& scenario = RaceData::GetStaticInstance()->menusScenario;
    
    u32 engine = holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_VS_CLASS);
    scenario.settings.engineClass = (EngineClass) engine;
    if(engine == Cosmos::Data::VS_CLASS_MIRROR){
        scenario.settings.engineClass = CC_150;
        scenario.settings.modeFlags |= 1;
    }
    else scenario.settings.modeFlags &= ~1;

    scenario.settings.cpuMode = (CpuMode) holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_VS_CPU);
    scenario.settings.itemMode = (ItemMode) holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_VS_ITEMS);

    u32 restriction = holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_VS_VEHICLES); 
    switch(restriction){
        case 0:
            context->vehicleRestriction = 2; break;
        case 1:
            context->vehicleRestriction = 0; break;
        default:
            context->vehicleRestriction = 1; break;
    }
    context->vsRaceCount = raceCounts[holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_VS_RACES)];
    holder->SetAllInnerSettings();
}
kmBranch(0x808529f8, ApplyVSSettings);

void OpenSettingsOnVS(Pages::VSModeSelect* menu, u32, PushButton& button){
    CosmosUI::NewSettings* page = CosmosUI::NewSettings::GetPage();
    COSMOS_ASSERT_NOT_NULL(page);

    page->SetNextSettingPage(Cosmos::Data::COSMOS_VS_SETTINGS_1);
    page->SetPreviousPage(VS_MODE_SELECT, SINGLE_PLAYER_FROM_MENU);
    menu->LoadNextPageWithDelayById((PageId)Cosmos::SETTINGS_MAIN, 0.0f);
}
kmCall(0x80852ab4, OpenSettingsOnVS);


