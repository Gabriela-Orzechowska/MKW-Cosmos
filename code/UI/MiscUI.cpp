#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>
#include <core/System/SystemManager.hpp>
#include <game/System/Archive.hpp>

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

static u32 defaultLanguage;

const char no[1] = "";

static char * suffixes[11] = {
    "",
    "_E.szs",
    "_U.szs",
    "_G.szs",
    "_F.szs",
    "_Q.szs",
    "_S.szs",
    "_M.szs",
    "_I.szs",
    "_J.szs",
    "_H.szs"
};

kmWriteRegionInstruction(0x80604094, 0x4800001c, 'E');

void UpdateLanguage()
{
    extern char * szsLanguageNames[7];

    u32 language = SettingsHolder::GetInstance()->GetSettings()->pages[DX_MENU_SETTINGS_1].setting[DX_LANGUAGE_SETTINGS];
    if(language == NO_CHANGE) language = defaultLanguage;
    SystemManager::sInstance->gameLanguage = language;
    char * localization = suffixes[language];
    if(language == NO_CHANGE) localization = szsLanguageNames[language];
    strncpy(ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archiveSuffixes[0x1], localization, 0x80);
    strncpy(ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archiveSuffixes[0x1], localization, 0x80);
}

void UpdateArchiveHolderLanguageOnInit()
{
    extern char * szsLanguageNames[7];

    defaultLanguage = SystemManager::sInstance->strapPageLanguage; //To include Dutch
    u32 language = SettingsHolder::GetInstance()->GetSettings()->pages[DX_MENU_SETTINGS_1].setting[DX_LANGUAGE_SETTINGS];
    if(language == NO_CHANGE) {
        if(defaultLanguage == 0x6)
            language = 0x10;
        else
            return;
    }
    SystemManager::sInstance->gameLanguage = language;
    char * localization = suffixes[language];
    strncpy(ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archiveSuffixes[0x1], localization, 0x80);
    strncpy(ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archiveSuffixes[0x1], localization, 0x80);
}

kmWrite32(0x8000ad9c, 0x38000006); //System Dutch

static SettingsUpdateHook UpdateSystemLanguage(UpdateLanguage);
//static MenuLoadHook UpdateSystemLanguageMenuLoad(UpdateLanguage);
//static StrapEndHook SetLanguageOnBoot(UpdateLanguage);
kmBranch(0x8053fc98, UpdateArchiveHolderLanguageOnInit);