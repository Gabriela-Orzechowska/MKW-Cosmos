#include "UI/Language/LanguageManager.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>
#include <core/System/SystemManager.hpp>
#include <game/System/Archive.hpp>
#include <game/UI/SectionMgr/SectionMgr.hpp>

#include <core/rvl/os/OS.hpp>
#include <UI/MiscUI.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <UI/BMG/MessageGroup.hpp>
#include <game/UI/Page/Other/Globe.hpp>


using namespace Cosmos::Data;

namespace CosmosUI
{
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const char * tpl){
        void * tplResource = ctrl->layout.resources->multiArcResourceAccessor.GetResource(res::RESOURCETYPE_TEXTURE, tpl);
        ChangePaneImage(ctrl, pane, tplResource);
        return;
    }

    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const void * tplPointer)
    {
        ctrl->layout.GetPaneByName(pane)->GetMaterial()->GetTexMapAry()->ReplaceImage((TPLPalettePtr) tplPointer);
        return;
    }

}

void FasterMenusOnTransition(MenuData *menuData, u32 delay, u32 r5){
    if (SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
        delay = 0;
    menuData->StartTransition(delay, r5);
}
kmCall(0x80602510, FasterMenusOnTransition);

void FasterPageTransition()
{
    float delay = 176.0f;
    if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
        delay = 0.0f;
    Page::transitionDelay = delay;
}
static SettingsUpdateHook FasterPages(FasterPageTransition);
static MenuLoadHook mhlFasterPages(FasterPageTransition);

kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');

kmWrite32(0x80007758, 0x981e0ccc);
kmWrite32(0x80007bc8, 0x4e800020);


void DisableFastMenuOnLicenseSelect() {
    Page::transitionDelay = 176.0f;
}
kmWritePointer(0x808b9888, DisableFastMenuOnLicenseSelect);
kmBranch(0x8063af8c, DisableFastMenuOnLicenseSelect);

void FixVSIntroBMG(LayoutUIControl* control){
    u32 trackBmg = GetCorrectTrackBMG(Cosmos::CupManager::GetStaticInstance()->GetTrackID());
    control->SetMsgId(trackBmg, nullptr);
}
kmCall(0x808552cc, FixVSIntroBMG);

void FixGPIntroBMG(LayoutUIControl& control, u32 bmg, TextInfo* info){
    u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
    if(trackId < 0x100) {
        control.SetMsgId(bmg, info);
        return;
    }
    u32 cupBmg = BMG_CUPS + Cosmos::CupManager::GetCupID(trackId);
    CosmosLog("Setting BMG to: %x from %x\n", cupBmg, info->bmgToPass[1]);
    info->bmgToPass[1] = cupBmg;
    control.SetMsgId(bmg, info);
}
kmCall(0x808553b4, FixGPIntroBMG);

void FixGPIntroIcon(LayoutUIControl& control, char* name, u32){
    u32 id = Cosmos::CupManager::GetCupID(Cosmos::CupManager::GetStaticInstance()->GetTrackID());

    char tpl[0x30];
    snprintf(tpl, 0x30, "button/timg/icon_cup_%03x.tpl", id);
    void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);
    CosmosUI::ChangePaneImage(&control, name, tplPointer);

}
kmCall(0x80855370, FixGPIntroIcon);

void AddPauseSceneToOnline(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage(VS_RACE_PAUSE_MENU);
}
kmCall(0x8062eccc, AddPauseSceneToOnline);
kmWrite32(0x808567c0, 0x60000000);
kmWrite32(0x808567c8, 0x60000000);
kmWrite16(0x808567cc+2, 0x0000);

void DontPauseOnline(void* unknown){
    GameMode mode = RaceData::GetStaticInstance()->racesScenario.GetSettings().gamemode;
    if(isOnline()) {
        Pages::RaceHUD* racehud = Pages::RaceHUD::sInstance;
        racehud->PlaySound(0xd3, -1);
        isOnlinePaused = true;
        return;
    }
    Scene::PauseGame(unknown);
}
kmCall(0x80856b38, DontPauseOnline);

void UnpauseOfflineSound(void* unknown){
    GameMode mode = RaceData::GetStaticInstance()->racesScenario.GetSettings().gamemode;
    if(isOnline()) {
        Pages::RaceHUD* racehud = Pages::RaceHUD::sInstance;
        racehud->PlaySound(0xd4, -1);
        isOnlinePaused = false;
        return;
    }
    Scene::UnpauseGame(unknown);
}
kmCall(0x8085a260, UnpauseOfflineSound);
kmCall(0x8085a200, UnpauseOfflineSound);
kmCall(0x8085a0dc, UnpauseOfflineSound);
kmCall(0x8085a080, UnpauseOfflineSound);

void ResetOnlineMenuState(){
    isOnlinePaused = false;
}
static RaceLoadHook rlhResetOnlineMenuState(ResetOnlineMenuState);

void CalcInput(RealControllerHolder& holder, bool isPaused){
    holder.UpdateFunc(isPaused);
    if(isPaused == false && isOnlinePaused) {
        holder.GetCurrentGameInputState().Reset();
        holder.GetPreviousGameInputState().Reset();            
    }
}
    

kmCall(0x80521784, CalcInput);

