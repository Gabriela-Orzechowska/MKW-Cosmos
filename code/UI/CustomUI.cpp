#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>
#include <game/UI/Page/RaceMenu/TTPause.hpp>
#include <System/Security.hpp>

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

void InjectSettingPage(Scene& scene, PageId id)
{
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    //
    return;
}
kmCall(0x8062fe3c, InjectSettingPage); //OPTIONS

//MenuSingle
kmCall(0x8062d334, InjectSettingPage); //From Main
kmCall(0x8062d478, InjectSettingPage); //From Change Char
kmCall(0x8062d5bc, InjectSettingPage); //From ChangeCourse
kmCall(0x8062d640, InjectSettingPage); //From Versus
kmCall(0x8062d6c4, InjectSettingPage); //From Battle
kmCall(0x8062d808, InjectSettingPage); //From Mission MOde

void InjectTTPages(Scene& scene, PageId id){
    InjectSettingPage(scene, id);
    scene.CreatePage(ARE_YOU_SURE_YOU_WANT_TO_QUIT);
};

kmCall(0x8062c644, InjectTTPages); // TTs

void InjectWarningPage(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::WARNING_PAGE);
    return;
}

kmCall(0x8062ce54, InjectWarningPage);
kmCall(0x8062ced8, InjectWarningPage);
kmCall(0x8062cf5c, InjectWarningPage);
kmCall(0x8062cfe0, InjectWarningPage);
kmCall(0x8062d064, InjectWarningPage);

static bool hasShownWarning = false;
void ShowCheatsWarningPage(Page& page, u32 id, float animLenght) {
    page.EndStateAnimate(animLenght,id);
    if(!hasShownWarning){
        if(Cosmos::Security::GeckoAnalizer::AreCheatsEnabled()){
            CosmosUI::MessagePageWindow* messagePage = MenuData::GetStaticInstance()->curScene->Get<CosmosUI::MessagePageWindow>((PageId)Cosmos::WARNING_PAGE);
            messagePage->AddMessage(CosmosUI::INFO, 0x2841);
            page.AddPageLayer((PageId)Cosmos::WARNING_PAGE, 0);
        }
        hasShownWarning = true;
    }
}
kmCall(0x8063b04c, ShowCheatsWarningPage);


