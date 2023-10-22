#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <UI/Settings/SettingsSubPage.hpp>

void * CreatePage(Scene * scene, u32 pageId)
{
    if(pageId == (u32)DX::SETTINGS_MAIN)
    {
        return new(DXUI::SettingsBasePage)();
    }
    else if(pageId == (u32)DX::RACE_SETTINGS1)
    {
        u8 optionCount[8] = {3, 2, 3, 2, 2, 2, 2, 2};
        u8 onOff[8] = {0,1,0,0,0,0,0,0};

        return new(DXUI::SettingSubPage)(2,optionCount,pageId,onOff);
    }
    
    else if(pageId == (u32)DX::MENU_SETTINGS1)
    {
        u8 optionCount[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        u8 onOff[8] = {1,0,0,0,0,0,0,0};

        return new(DXUI::SettingSubPage)(1,optionCount,pageId, onOff);
    }
    return scene->CreatePageById((PageId)pageId);
}

kmCall(0x80622d2c, CreatePage);