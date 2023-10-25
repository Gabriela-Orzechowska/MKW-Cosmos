#ifndef _DX_USER_DATA_
#define _DX_USER_DATA_

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>
#include <core/rvl/os/OS.hpp>
#include <main.hpp>

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

namespace DXData
{

#define PAGECOUNT 2

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
            static void SaveTask(void *);
            
        private:
            Settings * settings;
            static SettingsHolder * sInstance;
            void Init(char * filepath, const char * magic, u32 version);
            void RequestSave();
            char filepath[IPCMAXPATH];
    };

    enum SETTINGSPAGES{
        DX_RACE_SETTINGS_1 = 0x0,
        DX_MENU_SETTINGS_1 = 0x1,
    };

    enum RACE_SETTINGS_1_SETTINGS {
        DX_MUSIC_CUTOFF = 0x0,
        DX_DRAGGABLE_BLUES = 0x1,
    };

    enum MENU_SETTINGS_1_SETTINGS {
        DX_FAST_MENUS = 0x0,
    };
    
    enum MUSIC_CUTOFF_OPTIONS {
        SPEEDUP = 0x0,
        NO_CUTOFF = 0x1,
        CUTOFF_DISABLED = 0x2,
    };

    enum SETTINGS_BOOL {
        ENABLED = 0x0,
        DISABLED = 0x1,
    };


}

#endif