#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <UI/Settings/SettingsSubPage.hpp>

void * CreatePage(Scene * scene, u32 pageId)
{
    if(pageId == (u32)DX::SET_PAGE_1)
    {
        return new(DXUI::SettingsBasePage)();
    }
    else if(pageId == (u32)DX::SET_PAGE_2)
    {
        u8 optionCount[8] = {2, 2, 2, 2, 2, 2, 2, 2};

        return new(DXUI::SettingSubPage)(6,optionCount,1);
    }
    return scene->CreatePageById((PageId)pageId);
}

kmCall(0x80622d2c, CreatePage);