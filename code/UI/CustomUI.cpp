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


void InjectPage(Scene *scene, PageId id)
{
    scene->CreatePage(id);
    scene->CreatePage((PageId)DX::SETTINGS_MAIN);
    scene->CreatePage((PageId)DX::RACE_SETTINGS1);
    scene->CreatePage((PageId)DX::MENU_SETTINGS1);
    return;
}
kmCall(0x8062fe3c, InjectPage); //OPTIONS

//MenuSingle
kmCall(0x8062d334, InjectPage); //From Main
kmCall(0x8062d478, InjectPage); //From Change Char
kmCall(0x8062d5bc, InjectPage); //From ChangeCourse
kmCall(0x8062d640, InjectPage); //From Versus
kmCall(0x8062d6c4, InjectPage); //From Battle
kmCall(0x8062d808, InjectPage); //From Mission MOde

