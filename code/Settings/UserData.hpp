#ifndef _DX_USER_DATA_
#define _DX_USER_DATA_

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>

class SettingUpdateHook{
    private:
    typedef void (Func)();
    Func * func;
    SettingUpdateHook * mNext;

    static SettingUpdateHook * sHooks;

    public:
    SettingUpdateHook(Func * f)
    {
        mNext = sHooks;
        sHooks = this;
        func = f;
    }
    static void exec()
    {
        for(SettingUpdateHook * p = sHooks; p; p->mNext)
        {
            p->func();
        }
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

}

#endif