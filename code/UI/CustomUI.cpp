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
        DXUI::SettingPageDefinition definition;
        definition.settingCount = 3;
        definition.settings[0].isBool = false;
        definition.settings[0].optionCount = 3;
        definition.settings[1].isBool = true;
        definition.settings[1].optionCount = 2;
        definition.settings[2].isBool = false;
        definition.settings[2].optionCount = 3;
        if(DX::isDolphin) definition.settings[2].optionCount = 2;

        return new(DXUI::SettingSubPage)(&definition, pageId);
    }
    
    else if(pageId == (u32)DX::MENU_SETTINGS1)
    {

        DXUI::SettingPageDefinition definition;
        definition.settingCount = 2;
        definition.settings[0].isBool = true;
        definition.settings[0].optionCount = 2;
        definition.settings[1].isBool = false;
        definition.settings[1].optionCount = 13;

        return new(DXUI::SettingSubPage)(&definition ,pageId);
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
