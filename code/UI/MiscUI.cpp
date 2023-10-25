#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>

using namespace DXData;

void FasterMenusOnTransition(MenuData *menuData, u32 delay, u32 r5){
    OSReport("[DX] Transitioning...\n");
    if (SettingsHolder::GetInstance()->GetSettings()->pages[DX_MENU_SETTINGS_1].setting[DX_FAST_MENUS] == ENABLED) 
    {
        delay = 0;
        OSReport("[DX] Setting delay to 0...");
    }
    menuData->StartTransition(delay, r5);
}
kmCall(0x80602510, FasterMenusOnTransition);

void FasterPageTransition()
{
    float delay = 176.0f;
    if(SettingsHolder::GetInstance()->GetSettings()->pages[DX_MENU_SETTINGS_1].setting[DX_FAST_MENUS] == ENABLED) delay -= delay;
    Page::transitionDelay = delay;
}
static SettingsUpdateHook FasterPages(FasterPageTransition);

void FasterPageBoot()
{
    float delay = 176.0f;
    if(SettingsHolder::GetInstance()->GetSettings()->pages[DX_MENU_SETTINGS_1].setting[DX_FAST_MENUS] == ENABLED) delay -= delay;
    Page::transitionDelay = delay;
}

static BootHook FasterPagesBoot(FasterPageBoot, LOW);