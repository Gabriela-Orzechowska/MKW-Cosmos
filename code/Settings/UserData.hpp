#ifndef _COSMOS_USER_DATA_
#define _COSMOS_USER_DATA_

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>
#include <game/System/SaveDataManager.hpp>
#include <core/rvl/os/OS.hpp>
#include <main.hpp>

class SettingsUpdateHook
{
private:
    typedef void(Func)();
    Func *mFunc;
    SettingsUpdateHook *mNext;

    static SettingsUpdateHook *sHooks;

public:
    SettingsUpdateHook(Func *f)
    {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
    }

    static void exec()
    {
        for (SettingsUpdateHook *p = sHooks; p; p = p->mNext)
            p->mFunc();
    }
};

class SettingsValueUpdateHook
{
private:
    typedef void(Func)();
    Func *mFunc;
    SettingsValueUpdateHook *mNext;
    u32 setting;

    static SettingsValueUpdateHook *sHooks;

public:
    SettingsValueUpdateHook(Func *f, u32 settingIndex)
    {
        mNext = sHooks;
        sHooks = this;
        mFunc = f;
        setting = settingIndex;
    }

    static void exec(u32 settingIndex)
    {
        for (SettingsValueUpdateHook *p = sHooks; p; p = p->mNext)
            if (p->setting == settingIndex)
                p->mFunc();
    }
};

namespace Cosmos
{
    namespace Data
    {

#define SettingsVersion 8

        enum SETTINGSPAGES
        {
            COSMOS_RACE_SETTINGS_1 = 0x0,
            COSMOS_MENU_SETTINGS_1,
            COSMOS_DEBUG_SETTINGS,
            COSMOS_HOST_SETTINGS_1,
            COSMOS_VS_SETTINGS_1,
        };

        enum RACE_SETTINGS_1_SETTINGS
        {
            COSMOS_MUSIC_CUTOFF = 0x0,
            COSMOS_DRAGGABLE_BLUES,
            COSMOS_MII_HEADS,
            COSMOS_SPEEDOMETER,
            COSMOS_FRAME_MODE,
            COSMOS_GHOST_SAVING,
        };

        enum MENU_SETTINGS_1_SETTINGS
        {
            COSMOS_LANGUAGE_SETTINGS = 0x0,
            COSMOS_FAST_MENUS,
            COSMOS_SORTING,
        };

        enum DEBUG_SETTINGS
        {
            COSMOS_DWC_LOGS = 0x0,
            COSMOS_PERFORMANCE_MONITOR,
            COSMOS_DEBUG_MSGS,
            COSMOS_LOG_TO_SD,
        };

        enum HOST_SETTINGS_1
        {
            COSMOS_OPEN_HOST = 0x0,
            COSMOS_HAW,
            COSMOS_ALLOW_MII_HEADS,
            COSMOS_FORCE_CC,
            COSMOS_RACE_COUNT,
        };

        enum VS_SETTINGS {
            COSMOS_VS_CLASS = 0x0,
            COSMOS_VS_CPU,
            COSMOS_VS_VEHICLES,
            COSMOS_VS_COURSES,
            COSMOS_VS_ITEMS,
            COSMOS_VS_RACES,
        };

        enum LAYOUT_SETTINGS 
        {
            SORTING_DEFAULT = 0x0,
            SORTING_ALPHABETICAL,
        };

        enum MUSIC_CUTOFF_OPTIONS
        {
            SPEEDUP = 0x0,
            NO_CUTOFF,
            CUTOFF_DISABLED,
        };

        enum SPEEDOMETER_OPTIONS
        {
            SPEEDO_DISABLED = 0x0,
            SPEEDO_0_DIGIT,
            SPEEDO_1_DIGIT,
            SPEEDO_2_DIGIT,
        };

        enum FRAME_MODE
        {
            FRAME_MODE_DEFAULT = 0x0,
            FORCE_30 = 0x1,
            FORCE_60 = 0x2,
        };

        enum LANGUAGE_MODE
        {
            NO_CHANGE = 0x0,
        };

        enum SETTINGS_BOOL
        {
            ENABLED = 0x0,
            DISABLED = 0x1,
        };

        enum PERFORMANCE_MONITOR_MODE
        {
            PM_DISABLED = 0x0,
            PM_INRACE = 0x1,
            OM_ALWAYS = 0x2,
        };

        enum FORCE_CC
        {
            FORCE_NONE = 0x0,
            FORCE_150CC,
            FORCE_200CC,
            FORCE_MIRROR,
        };
        enum RACE_COUNTS
        {
            RACE_COUNT_2 = 0x0,
            RACE_COUNT_4,
            RACE_COUNT_8,
            RACE_COUNT_12,
            RACE_COUNT_16,
            RACE_COUNT_24,
            RACE_COUNT_32,
            RACE_COUNT_64,
        };

        enum VS_SETTINGS_OPTIONS {
            VS_CLASS_100 = 0x0,
            VS_CLASS_150 = 0x1,
            VS_CLASS_200 = 0x2,
            VS_CLASS_MIRROR = 0x3,
            VS_CPU_EASY = 0x0,
            VS_CPU_NORMAL = 0x1,
            VS_CPU_HARD = 0x2,
            VS_VEHICLES_ALL = 0x0,
            VS_VEHICLES_KARTS = 0x1,
            VS_VEHICLES_BIKES = 0x2,
            VS_COURSE_CHOOSE = 0x0,
            VS_COURSE_RANDOM = 0x1,
            VS_COURSE_INORDER = 0x2,
            VS_ITEM_RECOMMENDED = 0x0,
            VS_ITEM_FRANTIC = 0x1,
            VS_ITEM_BASIC = 0x2,
            VS_ITEM_NONE = 0x3,
        };

        enum GLOBAL_SETTING
        {
            COSMOS_SETTING_MUSIC_CUTOFF = COSMOS_MUSIC_CUTOFF + (COSMOS_RACE_SETTINGS_1 * 8),
            COSMOS_SETTING_DRAGGABLE_BLUES = COSMOS_DRAGGABLE_BLUES + (COSMOS_RACE_SETTINGS_1 * 8),
            COSMOS_SETTING_MII_HEADS = COSMOS_MII_HEADS + (COSMOS_RACE_SETTINGS_1 * 8),
            COSMOS_SETTING_SPEEDOMETER = COSMOS_SPEEDOMETER + (COSMOS_RACE_SETTINGS_1 * 8),
            COSMOS_SETTING_FRAME_MODE = COSMOS_FRAME_MODE + (COSMOS_RACE_SETTINGS_1 * 8),
            COSMOS_SETTING_GHOST_SAVING = COSMOS_GHOST_SAVING + (COSMOS_RACE_SETTINGS_1 * 8),

            COSMOS_SETTING_LANGUAGE_SETTINGS = COSMOS_LANGUAGE_SETTINGS + (COSMOS_MENU_SETTINGS_1 * 8),
            COSMOS_SETTING_FAST_MENUS = COSMOS_FAST_MENUS + (COSMOS_MENU_SETTINGS_1 * 8),
            COSMOS_SETTING_SORTING = COSMOS_SORTING + (COSMOS_MENU_SETTINGS_1 * 8),

            COSMOS_SETTING_DWC_LOGS = COSMOS_DWC_LOGS + (COSMOS_DEBUG_SETTINGS * 8),
            COSMOS_SETTING_PERFORMANCE_MONITOR = COSMOS_PERFORMANCE_MONITOR + (COSMOS_DEBUG_SETTINGS * 8),
            COSMOS_SETTING_DEBUG_MSGS = COSMOS_DEBUG_MSGS + (COSMOS_DEBUG_SETTINGS * 8),
            COSMOS_SETTING_LOG_TO_SD = COSMOS_LOG_TO_SD + (COSMOS_DEBUG_SETTINGS * 8),

            COSMOS_SETTING_OPEN_HOST = COSMOS_OPEN_HOST + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_HAW = COSMOS_HAW + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_ALLOW_MII_HEADS = COSMOS_ALLOW_MII_HEADS + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_FORCE_CC = COSMOS_FORCE_CC + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_RACE_COUNT = COSMOS_RACE_COUNT + (COSMOS_HOST_SETTINGS_1 * 8),


            COSMOS_SETTING_VS_CLASS = COSMOS_VS_CLASS + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_CPU = COSMOS_VS_CPU + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_VEHICLES = COSMOS_VS_VEHICLES + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_COURSES = COSMOS_VS_COURSES + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_ITEMS = COSMOS_VS_ITEMS + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_RACES = COSMOS_VS_RACES + (COSMOS_VS_SETTINGS_1 * 8),
        };

#define PAGE_COUNT 5
#define SETTINGS_PER_PAGE 8

        typedef struct SettingPageOption
        {
            u8 optionCount;
            bool isBool;
            u8 defaultValue;
            u32 nameBmg;
            u32 firstOptionBmg;
            u32 firstDescBmg;
        } SettingPageOption;

        typedef struct SettingPageDefinition
        {
            u8 settingCount;
            SettingPageOption settings[SETTINGS_PER_PAGE];
        } SettingPageDefinition;

        static SettingPageDefinition GlobalSettingDefinitions[PAGE_COUNT] = {
            {
                // Race
                .settingCount = 6,
                .settings = {{.optionCount = 3, .isBool = false, .defaultValue = SPEEDUP}, // Music Cutoff
                             {.optionCount = 2, .isBool = true, .defaultValue = ENABLED},  // Draggable Blues
                             {.optionCount = 2, .isBool = true, .defaultValue = DISABLED},  // Mii Heads
                             {.optionCount = 4, .isBool = false, .defaultValue = SPEEDO_1_DIGIT}, // Speedometer
                             {.optionCount = 3, .isBool = false, .defaultValue = FRAME_MODE_DEFAULT}, // Frame Mode
                             {.optionCount = 2, .isBool = true, .defaultValue = ENABLED}}  // Ghost Saving
            },
            {
                // Menu
                .settingCount = 3,
                .settings = {{.optionCount = 12, .isBool = false, .defaultValue = NO_CHANGE}, // Language //TODO REENABLE KOREAN
                             {.optionCount = 2, .isBool = true, .defaultValue = ENABLED},
                             {.optionCount = 2, .isBool = false, .defaultValue = SORTING_DEFAULT}},
            },
            {
                // Debug
                .settingCount = 4,
                .settings = {{.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, // DWC Logs
                             {.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, //
                             {.optionCount = 2, .isBool = true, .defaultValue = ENABLED},
                             {.optionCount = 2, .isBool = true, .defaultValue = ENABLED}} //LOG TO SD 
            },
            {// Host
             .settingCount = 5,
             .settings = {{.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, //OpenHost
                          {.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, // HAW
                          {.optionCount = 2, .isBool = true, .defaultValue = ENABLED}, //Allow Mii Heads
                          {.optionCount = 3, .isBool = false, .defaultValue = FORCE_NONE}, // Force CC
                          {.optionCount = 8, .isBool = false, .defaultValue = RACE_COUNT_4, .nameBmg = 0}}
            }, //Race count
            {
                .settingCount = 6,
                .settings = {
                    {.optionCount = 4, .isBool = false, .defaultValue = VS_CLASS_150, .nameBmg = 0xd52, .firstOptionBmg = 0xd53, .firstDescBmg = 0x0d57},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_CPU_NORMAL, .nameBmg = 0xd5c, .firstOptionBmg = 0xd5d, .firstDescBmg = 0xd61},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_VEHICLES_ALL, .nameBmg = 0xd66, .firstOptionBmg = 0xd67, .firstDescBmg = 0xd6a},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_COURSE_CHOOSE, .nameBmg = 0xd70, .firstOptionBmg = 0xd71, .firstDescBmg = 0xd74},
                    {.optionCount = 4, .isBool = false, .defaultValue = VS_ITEM_RECOMMENDED, .nameBmg = 0xd98, .firstOptionBmg = 0xd99, .firstDescBmg = 0xd9d},
                    {.optionCount = 7, .isBool = false, .defaultValue = RACE_COUNT_4, .nameBmg = 0xd52, .firstOptionBmg = 0x3341},
                }
            },
        };

        static u8 GlobalSettingsPageOrder[PAGE_COUNT] = {COSMOS_MENU_SETTINGS_1, COSMOS_RACE_SETTINGS_1, COSMOS_VS_SETTINGS_1, COSMOS_HOST_SETTINGS_1, COSMOS_DEBUG_SETTINGS};

        struct SettingsPage
        {
            u8 setting[SETTINGS_PER_PAGE];
        };

        class Settings
        {
        public:
            Settings(){};

            char signature[4];
            u32 version;
            union
            {
                SettingsPage pages[PAGE_COUNT];
                u8 rawSettings[PAGE_COUNT * SETTINGS_PER_PAGE];
            } data[4];
            u16 playerVr[4];
            u16 playerBr[4];
        } __attribute__((aligned(0x20)));

        class SettingsHolder
        {
        public:
            SettingsHolder();
            static void Create();
            static SettingsHolder *GetStaticInstance();

            u8 GetSettingValue(GLOBAL_SETTING setting) const { return this->settings->data[currentLicense].rawSettings[setting]; }
            u8 GetSettingValue(u8 page, u8 setting) const { return this->settings->data[currentLicense].pages[page].setting[setting]; }

            void SetSettingValue(u8 value, GLOBAL_SETTING setting) { this->settings->data[currentLicense].rawSettings[setting] = value; }
            void SetSettingValue(u8 value, u8 page, u8 setting) { this->settings->data[currentLicense].pages[page].setting[setting] = value; }

            void Update();
            void Save();

            u32 GetUserVR() const { return GetUserVR(currentLicense); }
            u32 GetUserVR(u32 id) const { return this->settings->playerVr[id]; }
            u32 GetUserBR() const { return GetUserBR(currentLicense); }
            u32 GetUserBR(u32 id) const { return this->settings->playerBr[id]; }

            void SetUserVR(u32 value) { SetUserVR(value, currentLicense); }
            void SetUserVR(u32 value, u32 id) { this->settings->playerVr[id] = value; }
            void SetUserBR(u32 value) { SetUserVR(value, currentLicense); }
            void SetUserBR(u32 value, u32 id) { this->settings->playerBr[id] = value; }

            inline bool AreMiiHeadsAllowed() { return miiHeadsEnabled; }
            inline void SetMiiHeadSettings(bool setting) { miiHeadsEnabled = setting; }
            void SetCurrentLicense(int i) { this->currentLicense = i; }
            static void SaveTask(void *);

        private:
            Settings *settings;
            static SettingsHolder *sInstance;
            void Init(const char *filepath, const char *magic, u32 version);
            void RequestSave();
            char filepath[IPCMAXPATH];

            bool miiHeadsEnabled;
            int currentLicense;
        };
    }
}

#endif
