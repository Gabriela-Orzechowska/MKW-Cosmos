#pragma once
#include <kamek.hpp>
#include <core/System/RKSystem.hpp>
#include <core/System/SystemManager.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <UI/Settings/NewSettingsPage.hpp>

extern char * szsLanguageNames[7];

namespace Cosmos
{
    class LanguageManager {
    enum Languages {
        DEFAULT = 0x0,
        ENGLISH,
        ENGLISH_USA,
        GERMAN,
        FRENCH,
        FRENCH_QUEBEC,
        SPANISH,
        SPANISH_MEXICO,
        ITALIAN,
        DUTCH,
        POLISH,
        JAPANESE,
        KOREAN,
        NUMBER_OF_LANGUAGES,
    };  
    enum GameLanguages {
        P_NONE,
        P_ENGLISH,
        P_GERMAN,
        P_FRENCH,
        P_SPANISH,
        P_ITALIAN,
        P_DUTCH,
    };

    public:
        static LanguageManager* GetStaticInstance() { return sInstance; }
        static void CreateStaticInstance();

        void Init();
        void Update(bool reload);
        u32 GetActualLanguage();

        u32 GameToGlobal(u32 i, u32 a);

    private:
        static LanguageManager* sInstance;
        u32 systemLanguage;
        u32 currentLanguageOption;
        u32 actualLanguage;
        u32 lastLanguage;
        bool isDefault;
        bool isKorean;
        bool needsUpdate;
        bool isBoot;
    };

    static char* suffixes[13] = {
        "",
        "_E",
        "_U",
        "_G",
        "_F",
        "_Q",
        "_S",
        "_M",
        "_I",
        "_H",
        "_PL",
        "_J",
        "_K",
    };
} // namespace Cosmos
