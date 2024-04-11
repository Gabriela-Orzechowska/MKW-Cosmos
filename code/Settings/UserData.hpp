#ifndef _COSMOS_USER_DATA_
#define _COSMOS_USER_DATA_

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>
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

namespace CosmosData
{

#define PAGECOUNT 3

    struct SettingsPage{
        u8 setting[8];
    };
    class SettingBase {};

    class Settings : SettingBase{
        public:
        Settings(){};
        u8 GetSettingValue(GLOBAL_SETTING setting) const { return this->settings.rawSettings[setting]; }
        u8 GetSettingValue(u8 page, u8 setting) const { return this->settings.pages[page].setting[setting]; }

        char signature[4];
        u32 version;
        union {
            SettingsPage pages[PAGECOUNT];
            u8 rawSettings[PAGECOUNT * 8];
        } settings;
        u16 playerVr[4];
        u16 playerBr[4];
    } __attribute__ ((aligned(0x20)));

    class SettingsHolder
    {
        public:
            SettingsHolder();
            static void Create();
            static SettingsHolder * GetInstance();
            Settings * GetSettings(){return this->settings;};
            void Update();
            void Save();
            u32 GetUserVR();
            u32 GetUserVR(u32 id);
            static void SaveTask(void *);
            
        private:
            Settings * settings;
            static SettingsHolder * sInstance;
            void Init(char * filepath, const char * magic, u32 version);
            void RequestSave();
            char filepath[IPCMAXPATH];
    };

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
        COSMOS_SETTING_DWC_LOGS = 0x0 + (COSMOS_DEBUG_SETTINGS_1 * 8),
        COSMOS_SETTING_PERFORMANCE_MONITOR = 0x1 + (COSMOS_DEBUG_SETTINGS_1 * 8),
    };
}

#endif