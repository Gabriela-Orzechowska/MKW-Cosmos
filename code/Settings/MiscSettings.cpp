#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/Scene/BaseScene.hpp>
#include <Debug/Draw/PerformanceMonitor.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Other/LicenseSettings.hpp>

using namespace CosmosData;

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

static SettingsValueUpdateHook svuhFramerate(SetFrameRateMidGame, CosmosData::COSMOS_SETTING_FRAME_MODE);

void SetPerfMonitorVisibility()
{
    ((CosmosDebug::PerformanceMonitor *) RKSystem::mInstance.processMeter)->visible = (SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_PERFORMANCE_MONITOR) == ENABLED);
}
static SettingsUpdateHook suhSetPerfMonitorVisibility(SetPerfMonitorVisibility);
static SettingsValueUpdateHook svuhSetPerfMonitorVisibility(SetPerfMonitorVisibility, CosmosData::COSMOS_SETTING_PERFORMANCE_MONITOR);
static BootHook bhSetPerfMonitorVisibility(SetPerfMonitorVisibility, LOW);

extern u32 DWC_ReportLevel;
void SetDWCLogs(){
    DWC_ReportLevel = 0x0;
    if(SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_DWC_LOGS) == ENABLED) {
        DWC_ReportLevel = ~0x0;
    }
}
static SettingsUpdateHook suhDWCLogs(SetDWCLogs);
static BootHook bhDWCLogs(SetDWCLogs, LOW);
kmWrite32(0x80658be8, 0x60000000);

extern "C" {
    u64 DWC_CreateFriendKey(void * val);
}

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
        LicenseManager& currentLicense = SaveDataManager::GetStaticInstance()->licenses[licenseIndex];
        u64 val = DWC_CreateFriendKey(&currentLicense.friendCodeThing);
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
//kmCall(0x805eacf0, PatchLicenseButtonFile);

kmBranch(0x805eae94, SetupLicenseButton);