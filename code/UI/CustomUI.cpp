#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <UI/Settings/SettingsSubPage.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>

void * CreatePage(Scene& scene, u32 pageId)
{
    if(pageId == (u32)Cosmos::SETTINGS_MAIN)
    {
        return new(CosmosUI::NewSettings)();
    }
    else if(pageId == (u32)Cosmos::RACE_SETTINGS1)
    {
        return new(CosmosUI::SettingSubPage)(pageId, 0);
    }
    
    else if(pageId == (u32)Cosmos::MENU_SETTINGS1)
    {
        return new(CosmosUI::SettingSubPage)(pageId, 1);
    }
    else if(pageId == (u32)Cosmos::DEBUG_SETTINGS)
    {
        return new(CosmosUI::SettingSubPage)(pageId, 2);
    }
    else if(pageId == (u32)Cosmos::WARNING_PAGE)
    {
        return new (CosmosUI::MessagePageWindow);
    }

    return scene.CreatePageById((PageId)pageId);
}

kmCall(0x80622d2c, CreatePage);


void InjectPage(Scene& scene, PageId id)
{
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
   // scene.CreatePage((PageId)Cosmos::RACE_SETTINGS1);
   // scene.CreatePage((PageId)Cosmos::MENU_SETTINGS1);
   // scene.CreatePage((PageId)Cosmos::DEBUG_SETTINGS);
    scene.CreatePage((PageId)Cosmos::WARNING_PAGE);
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

