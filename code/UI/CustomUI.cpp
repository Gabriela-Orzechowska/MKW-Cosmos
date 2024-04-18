#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>
#include <game/UI/Page/RaceMenu/TTPause.hpp>

void * CreatePage(u32 pageId)
{
    if(pageId == (u32)Cosmos::SETTINGS_MAIN)
    {
        return new(CosmosUI::NewSettings)();
    }
    else if(pageId == (u32)Cosmos::WARNING_PAGE)
    {
        return new (CosmosUI::MessagePageWindow);
    }

    return Scene::CreatePageById((PageId)pageId);
}

kmCall(0x80622d2c, CreatePage);


void InjectPage(Scene& scene, PageId id)
{
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    // scene.CreatePage((PageId)Cosmos::WARNING_PAGE);
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

kmCall(0x8062c644, InjectPage); // TTs
/*
void TestSettings(Pages::TTPause& pauseMenu){
    CosmosUI::NewSettings::SetPreviousPageGlobal(TIME_TRIAL_PAUSE_MENU, TIME_TRIAL_GAMEPLAY);
    pauseMenu.nextPage = (PageId) Cosmos::SETTINGS_MAIN;
    pauseMenu.EndStateAnimate(0.0f,0);
}
kmCall(0x8085a5d8, TestSettings);

*/
