#ifndef _COSMOS_USER_DATA_
#define _COSMOS_USER_DATA_

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>
#include <game/System/SaveDataManager.hpp>
#include <core/rvl/os/OS.hpp>
#include <main.hpp>

static u32 settingsPageIds[] = {Cosmos::RACE_SETTINGS1, Cosmos::MENU_SETTINGS1, Cosmos::DEBUG_SETTINGS};

class SettingsUpdateHook {
private:
    typedef void (Func)();
    Func *mFunc;
    SettingsUpdateHook * mNext;

    static SettingsUpdateHook * sHooks;

public:
    SettingsUpdateHook(Func * f) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec() {
        for (SettingsUpdateHook * p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

class SettingsValueUpdateHook {
private:
    typedef void (Func)();
    Func *mFunc;
    SettingsValueUpdateHook * mNext;
    u32 setting;

    static SettingsValueUpdateHook * sHooks;

public:
    SettingsValueUpdateHook(Func * f, u32 settingIndex) {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
        setting = settingIndex;
    }

    static void exec(u32 settingIndex) {
        for (SettingsValueUpdateHook * p = sHooks; p; p = p->mNext)
            if(p->setting == settingIndex)
                p->mFunc();
    }
};

namespace CosmosData
{
    enum SETTINGSPAGES{
        COSMOS_RACE_SETTINGS_1 = 0x0,
        COSMOS_MENU_SETTINGS_1 = 0x1,
        COSMOS_DEBUG_SETTINGS = 0x2,
    };

    enum RACE_SETTINGS_1_SETTINGS {
        COSMOS_MUSIC_CUTOFF = 0x0,
        COSMOS_DRAGGABLE_BLUES = 0x1,
        COSMOS_FRAME_MODE = 0x2,
    };

    enum MENU_SETTINGS_1_SETTINGS {
        COSMOS_LANGUAGE_SETTINGS = 0x0,
        COSMOS_FAST_MENUS = 0x1,
        COSMOS_LAYOUT = 0x2,
    };
    
    enum DEBUG_SETTINGS {
        COSMOS_DWC_LOGS = 0x0,
        COSMOS_PERFORMANCE_MONITOR = 0x1,
    };

    enum MUSIC_CUTOFF_OPTIONS {
        SPEEDUP = 0x0,
        NO_CUTOFF = 0x1,
        CUTOFF_DISABLED = 0x2,
    };


    enum FRAME_MODE{
        FRAME_MODE_DEFAULT = 0x0,
        FORCE_30 = 0x1,
        FORCE_60 = 0x2,
    };

    enum LANGUAGE_MODE{
        NO_CHANGE = 0x0,
    };

    enum SETTINGS_BOOL {
        ENABLED = 0x0,
        DISABLED = 0x1,
    };

    enum PERFORMANCE_MONITOR_MODE{
        PM_DISABLED = 0x0,
        PM_INRACE = 0x1,
        OM_ALWAYS = 0x2,
    };

        
    enum GLOBAL_SETTING {
        COSMOS_SETTING_MUSIC_CUTOFF = 0x0 + (COSMOS_RACE_SETTINGS_1 * 8),
        COSMOS_SETTING_DRAGGABLE_BLUES = 0x1 + (COSMOS_RACE_SETTINGS_1 * 8),
        COSMOS_SETTING_FRAME_MODE = 0x2 + (COSMOS_RACE_SETTINGS_1 * 8),
        COSMOS_SETTING_LANGUAGE_SETTINGS = 0x0 + (COSMOS_MENU_SETTINGS_1 * 8),
        COSMOS_SETTING_FAST_MENUS = 0x1 + (COSMOS_MENU_SETTINGS_1 * 8),
        COSMOS_SETTING_LAYOUT = 0x2 + (COSMOS_MENU_SETTINGS_1 * 8),
        COSMOS_SETTING_DWC_LOGS = 0x0 + (COSMOS_DEBUG_SETTINGS * 8),
        COSMOS_SETTING_PERFORMANCE_MONITOR = 0x1 + (COSMOS_DEBUG_SETTINGS * 8),
    };


#define PAGE_COUNT 3
#define SETTINGS_PER_PAGE 8

    typedef struct SettingPageOption{
        u8 optionCount;
        bool isBool;
        u8 defaultValue;
    } SettingPageOption;

    typedef struct SettingPageDefinition{
        u8 settingCount;
        SettingPageOption settings[SETTINGS_PER_PAGE];
    } SettingPageDefinition;

    static SettingPageDefinition GlobalSettingDefinitions[PAGE_COUNT] = {
        {
            .settingCount = 3,
            .settings = { { .optionCount = 3, .isBool = false, .defaultValue = 0 }, 
            { .optionCount = 2, .isBool = true, .defaultValue = 0 },
            { .optionCount = 3, .isBool = false, .defaultValue = 0}  }
        }, {
            .settingCount = 2,
            .settings = { { .optionCount = 13, .isBool = false, .defaultValue = 0 }, 
            { .optionCount = 2, .isBool = true, .defaultValue = 0 }}
        }, { 
            .settingCount = 2,
            .settings = {{ .optionCount = 2, .isBool = true, .defaultValue = 1 },
            { .optionCount = 2, .isBool = true, .defaultValue = 1 }}
        }
    } ;

    struct SettingsPage{
        u8 setting[SETTINGS_PER_PAGE];
    };
    class SettingBase {};

    class Settings : SettingBase{
        public:
        Settings(){};

        char signature[4];
        u32 version;
        union {
            SettingsPage pages[PAGE_COUNT];
            u8 rawSettings[PAGE_COUNT * SETTINGS_PER_PAGE];
        } data;
        u16 playerVr[4];
        u16 playerBr[4];
    } __attribute__ ((aligned(0x20)));

    class SettingsHolder
    {
        public:
            SettingsHolder();
            static void Create();
            static SettingsHolder * GetInstance();

            u8 GetSettingValue(GLOBAL_SETTING setting) const { return this->settings->data.rawSettings[setting]; }
            u8 GetSettingValue(u8 page, u8 setting) const { return this->settings->data.pages[page].setting[setting]; }

            void SetSettingValue(u8 value, GLOBAL_SETTING setting) { this->settings->data.rawSettings[setting] = value; }
            void SetSettingValue(u8 value, u8 page, u8 setting) { this->settings->data.pages[page].setting[setting] = value; }

            void Update();
            void Save();

            u32 GetUserVR() const { return GetUserVR(SaveDataManager::sInstance->curLicenseId); }
            u32 GetUserVR(u32 id) const { return this->settings->playerVr[id]; }
            u32 GetUserBR() const { return GetUserBR(SaveDataManager::sInstance->curLicenseId); }
            u32 GetUserBR(u32 id) const { return this->settings->playerBr[id]; }

            void SetUserVR(u32 value) { SetUserVR(value, SaveDataManager::sInstance->curLicenseId); }
            void SetUserVR(u32 value, u32 id) { this->settings->playerVr[id] = value; }
            void SetUserBR(u32 value) { SetUserVR(value, SaveDataManager::sInstance->curLicenseId); }
            void SetUserBR(u32 value, u32 id) { this->settings->playerBr[id] = value; }

            static void SaveTask(void *);
            
        private:
            Settings * settings;
            static SettingsHolder * sInstance;
            void Init(char * filepath, const char * magic, u32 version);
            void RequestSave();
            char filepath[IPCMAXPATH];
    };
}

#endif