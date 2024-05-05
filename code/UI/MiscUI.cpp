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

static u32 defaultLanguage;
static u32 lastLanguage;

static char * suffixes[13] = {
    "",
    "_E.szs",
    "_U.szs",
    "_G.szs",
    "_F.szs",
    "_Q.szs",
    "_S.szs",
    "_M.szs",
    "_I.szs",
    "_H.szs",
    "_PL.szs",
    "_J.szs",
    "_K.szs",
};

kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');


static u32 fontFileName = 0x0;
static u32 fontBRFNTName = 0x0;

const char koreanFontFile[21] = "/Scene/UI/Font_K";
const char koreanFont[22] = "kart_font_korea.brfnt";

static char * normalFontFile = (char *) 0x808904a8;
static char * normalFont = (char *) 0x808b87ac;

static EGG::Heap * fontHeap;

void UpdateLanguage()
{
    extern char * szsLanguageNames[7];

    u32 language = SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_LANGUAGE_SETTINGS);
    u32 languageSettings = language;
    if(language == NO_CHANGE) language = defaultLanguage;
    SystemManager::sInstance->gameLanguage = language;
    char * localization = suffixes[language];
    if(language == NO_CHANGE) localization = szsLanguageNames[language];
    strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->archiveSuffixes[0x1], localization, 0x80);
    strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_COMMON)->archiveSuffixes[0x1], localization, 0x80);
/*
    CosmosUI::SettingsBasePage * page = MenuData::GetStaticInstance()->curScene->Get<CosmosUI::SettingsBasePage>((PageId)Cosmos::SETTINGS_MAIN);
    if((language == 12) == loadedAsKorean)
    {
        if(languageSettings != lastLanguage)
        {
            lastLanguage = languageSettings;
            page->wasLanguageChanged = true;
            //MenuData::GetStaticInstance();
            //page->LoadPrevPageWithDelayById(page->lastPage, 100.0f);
            
        }
    }
    else
    {
        page->koreanChange = true;
    }
*/
    if(languageSettings != lastLanguage){
        lastLanguage = languageSettings;
        if(Scene::GetType(MenuData::GetStaticInstance()->curScene->menuId) != CATEGORY_GAMEPLAY)
        {
            CosmosUI::NewSettings * page = MenuData::GetStaticInstance()->curScene->Get<CosmosUI::NewSettings>((PageId)Cosmos::SETTINGS_MAIN);
            page->ChangeMenu(page->GetPreviousMenu(),0,0.0f);
        }
    }
}

void UpdateArchiveHolderLanguageOnInit()
{
    extern char * szsLanguageNames[7];

    defaultLanguage = SystemManager::sInstance->strapPageLanguage; //To include Dutch
    u32 language = SettingsHolder::GetInstance()->GetSettingValue(COSMOS_SETTING_LANGUAGE_SETTINGS);
    lastLanguage = language;
    if(language == NO_CHANGE) {
        if(defaultLanguage == 0x6)
        {
            language = 0x10;
            lastLanguage = language;
        }
        else
            return;
    }
    
    SystemManager::sInstance->gameLanguage = language;
    char * localization = suffixes[language];
    strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->archiveSuffixes[0x1], localization, 0x80);
    strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_COMMON)->archiveSuffixes[0x1], localization, 0x80);

    fontFileName = Cosmos::GetPortAddress(0x808b396c, 0x808af11c, 0x808b2acc, 0x808a1de4);
    fontBRFNTName = Cosmos::GetPortAddress(0x808b87ac, 0x808ced94, 0x808b78fc, 0x808a6c14);

    if(language == 12)
    {
        loadedAsKorean = true;
        *(u32 *)(fontFileName) = (u32)&koreanFontFile;
        *(u32 *)(fontBRFNTName) = (u32)&koreanFont;
    }

    
}

kmWrite32(0x8000ad9c, 0x38000006); //System Dutch

static SettingsUpdateHook UpdateSystemLanguage(UpdateLanguage);
static SettingsUpdateHook FasterPages(FasterPageTransition);

kmBranch(0x8053fc98, UpdateArchiveHolderLanguageOnInit);

void DisableBMGSizeOverride()
{
    extern u32 p_bmgSizeAddress;
    p_bmgSizeAddress = 0xa01d004c; //lhz r0,0x4c(r29)
}

static LeCodeLoadHook fuckingbmgFix(DisableBMGSizeOverride);

void WhiteStrapTransition()
{
    nw4r::ut::Color white;
    white.rgba = 0xFFFFFFFF;
    RKSystem::sInstance->sceneManager->colorFader->setColor(white);
}

static BootHook hWhiteStrapTransition(WhiteStrapTransition, LOW);

