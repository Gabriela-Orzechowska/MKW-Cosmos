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

kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');

static SettingsUpdateHook FasterPages(FasterPageTransition);


void WhiteStrapTransition()
{
    nw4r::ut::Color white;
    white.rgba = 0xFFFFFFFF;
    RKSystem::sInstance->sceneManager->colorFader->setColor(white);
}

kmWrite32(0x80007758, 0x981e0ccc);
kmWrite32(0x80007bc8, 0x4e800020);
//static BootHook hWhiteStrapTransition(WhiteStrapTransition, LOW);

