#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>
#include <core/System/SystemManager.hpp>
#include <game/System/Archive.hpp>
#include <game/UI/SectionMgr/SectionMgr.hpp>

#include <game/System/FontManager.hpp>
#include <core/rvl/os/OS.hpp>
#include <UI/MiscUI.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <SlotExpansion/SlotExpansion.hpp>
#include <UI/BMG/MessageGroup.hpp>

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
    if (SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
        delay = 0;
    menuData->StartTransition(delay, r5);
}
kmCall(0x80602510, FasterMenusOnTransition);

void FasterPageTransition()
{
    float delay = 176.0f;
    if(SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
        delay = 0.0f;
    Page::transitionDelay = delay;
}

void FasterPageBoot()
{
    float delay = 176.0f;
    if(SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
        delay = 0.0f;
    Page::transitionDelay = delay;
}

static BootHook FasterPagesBoot(FasterPageBoot, LOW);
static SettingsUpdateHook FasterPages(FasterPageTransition);

kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');

kmWrite32(0x80007758, 0x981e0ccc);
kmWrite32(0x80007bc8, 0x4e800020);

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
    u32 cupBmg = BMG_CUPS + ((trackId - 0x100) / 4) + 0x8;
    CosmosLog("Setting BMG to: %x from %x\n", cupBmg, info->bmgToPass[1]);
    info->bmgToPass[1] = cupBmg;
    control.SetMsgId(bmg, info);
}
kmCall(0x808553b4, FixGPIntroBMG);

