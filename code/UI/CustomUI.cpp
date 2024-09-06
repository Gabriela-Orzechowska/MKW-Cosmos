#include "Debug/Debug.hpp"
#include "Debug/IOSDolphin.hpp"
#include "System/Identifiers.hpp"
#include "UI/CupSelect/CourseSelect.hpp"
#include "game/System/identifiers.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <main.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>
#include <game/UI/Page/RaceMenu/TTPause.hpp>
#include <game/UI/Page/Menu/TopMenuOverlay.hpp>
#include <System/Security.hpp>
#include <Aurora/AuroraSecurity.hpp>
#include <Aurora/AuroraAPI.hpp>

void * CreatePage(u32 pageId)
{
    if(pageId == (u32)Cosmos::SETTINGS_MAIN)
    {
        return new(CosmosUI::NewSettings);
    }
    else if(pageId == (u32)Cosmos::WARNING_PAGE)
    {
        return new (CosmosUI::MessagePageWindow);
    }
    else if(pageId == (u32)Cosmos::VARIANT_SELECT){
        return new (CosmosUI::VariantSelectPlus);
    }
    else if(pageId == (u32)Cosmos::SPINNER_WITH_BACKGROUND){
        return new (CosmosUI::AwaitPageWithBackground);
    }
    return Scene::CreatePageById((PageId)pageId);
}

kmCall(0x80622d2c, CreatePage);

void InjectOnlinePages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::VARIANT_SELECT);
}
kmCall(0x8062e864, InjectOnlinePages);
kmCall(0x8062e0d8, InjectOnlinePages);
kmCall(0x8062e744, InjectOnlinePages);
kmCall(0x8062eaa4, InjectOnlinePages);

void InjectMenuSinglePages(Scene& scene, PageId id)
{
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    scene.CreatePage((PageId)Cosmos::VARIANT_SELECT);
    return;
}

//MenuSingle
kmCall(0x8062d334, InjectMenuSinglePages); //From Main
kmCall(0x8062d478, InjectMenuSinglePages); //From Change Char
kmCall(0x8062d5bc, InjectMenuSinglePages); //From ChangeCourse
kmCall(0x8062d640, InjectMenuSinglePages); //From Versus
kmCall(0x8062d6c4, InjectMenuSinglePages); //From Battle
kmCall(0x8062d808, InjectMenuSinglePages); //From Mission MOde

void InjectGhostPages(Scene& scene, PageId id) {
    scene.CreatePage(TIME_TRIAL_INTERFACE);
    scene.CreatePage(SPLITS_AFTER_TT);


}

kmCall(0x8062ccd4, InjectGhostPages);
kmCall(0x8062cc5c, InjectGhostPages);
kmCall(0x8062cc98, InjectGhostPages);

void InjectTTPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    scene.CreatePage(ARE_YOU_SURE_YOU_WANT_TO_QUIT);
    scene.CreatePage(VOTERANDOM_MESSAGE_BOX);
    scene.CreatePage(TEXT_BOX_WITH_SPINNER);
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

void InjectMainMenuPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SPINNER_WITH_BACKGROUND);
    scene.CreatePage(GENERIC_TEXT_BOX_FULL_PAGE_PRESS_A);
    return;
}
kmCall(0x8062ce78, InjectMainMenuPages);
kmCall(0x8062cefc, InjectMainMenuPages);
kmCall(0x8062cf80, InjectMainMenuPages);
kmCall(0x8062d004, InjectMainMenuPages);
kmCall(0x8062d088, InjectMainMenuPages);

#define MIN_DOLPHIN_VERSION 17856

static bool hasShownWarning = false;
void ShowCheatsWarningPage(Page& page, u32 id, float animLenght) {

    if(!hasShownWarning){
        bool warningAdded = false;
        CosmosUI::MessagePageWindow* messagePage = MenuData::GetStaticInstance()->curScene->Get<CosmosUI::MessagePageWindow>((PageId)Cosmos::WARNING_PAGE);
        if(CosmosDebug::currentPlatform <= CosmosDebug::DOLPHIN_UNKNOWN && IOS::Dolphin::GetNumericalVersionNumber() < MIN_DOLPHIN_VERSION){
            messagePage->AddMessage(CosmosUI::SHUTDOWN, 0x2806);
            warningAdded = true;
        }
        else {
            page.EndStateAnimate(animLenght,id);
        }
        if(Cosmos::Security::GeckoAnalizer::AreCheatsEnabled()){
            messagePage->AddMessage(CosmosUI::INFO, 0x2841);
            warningAdded = true;
        }
        if((*(u32*)0x8000311C) > 0x04010000) {
            messagePage->AddMessage(CosmosUI::INFO, 0x2842);
            warningAdded = true;
        }
        if(Aurora::Security::IsDefaultDolphin() == Aurora::Security::DEFAULT){
            messagePage->AddMessage(CosmosUI::INFO, 0x2843);
            warningAdded = true;
        }
        else if(Aurora::Security::IsDefaultDolphin() == Aurora::Security::OVERUSED){
            messagePage->AddMessage(CosmosUI::INFO, 0x2844);
            warningAdded = true;
        }
        if(warningAdded) page.AddPageLayer((PageId)Cosmos::WARNING_PAGE, 0);

        hasShownWarning = true;
    }
    else page.EndStateAnimate(animLenght, id);

}
kmCall(0x8063b04c, ShowCheatsWarningPage);


static int FontDotSize = 1;

asm int AdjustFontSize() {
    ASM (
        nofralloc;
        lis r6, FontDotSize@ha;
        lwz r6, FontDotSize@l (r6);
        mr r0, r6;
        blr;
    );
}
kmCall(0x80022278, AdjustFontSize);

static bool destroyed = false;
typedef void (*Console_Destroy)();
void DestroyConsole(){
    if(destroyed) return;
    Console_Destroy destroy = (Console_Destroy) *((u32*)0x80003fE8);
    destroy();
    #ifdef COSMOS_SECURITY
    Cosmos::Security::LoaderCleanup();
    #endif
    destroyed = true;
    FontDotSize = 2;
}
kmBranch(0x800074a0, DestroyConsole);


