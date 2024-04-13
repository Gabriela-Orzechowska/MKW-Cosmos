#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/Scene/BaseScene.hpp>
#include <Debug/Draw/PerformanceMonitor.hpp>

using namespace CosmosData;

void SetFrameRate(GameScene& scene, u8 mode)
{
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FRAME_MODE);

    if(frameMode == FORCE_30) mode = 1;
    else if(frameMode == FORCE_60) mode = 0;

    scene.SetFramerate(mode);
}

kmCall(0x80554270, SetFrameRate);
kmCall(0x80554abc, SetFrameRate);

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