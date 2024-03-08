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
            char signature[4];
            u32 version;
            SettingsPage pages[PAGECOUNT];
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


}

#endif