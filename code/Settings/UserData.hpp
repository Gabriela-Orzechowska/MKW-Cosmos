/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COSMOS_USER_DATA_
#define _COSMOS_USER_DATA_

#include "UI/BMG/BMG.hpp"
#include <kamek.hpp>
#include <FileManager/FileManager.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/Race/RaceData.hpp>
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

#define SettingsVersion 10

        enum SETTINGSPAGES
        {
            COSMOS_RACE_SETTINGS_1 = 0x0,
            COSMOS_MENU_SETTINGS_1,
            COSMOS_DEBUG_SETTINGS,
            COSMOS_HOST_SETTINGS_1,
            COSMOS_VS_SETTINGS_1,
            COSMOS_VS_SETTINGS_2,
            COSMOS_HOST_SETTINGS_2,
            AURORA_ACCESIBILITY_SETTINGS_1,
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
            COSMOS_HOST_VARIANT_SELECTION,
            COSMOS_FORCE_CC,
            COSMOS_RACE_COUNT,
            AURORA_HOST_TRACK_LIST = 0x0,
            COSMOS_HOST_MEGA_TC = 0x1,
            COSMOS_HOST_ALL_ITEMS_CAN_LAND = 0x2,
            AURORA_HOST_RANDOM_FORCED = 0x3,
        };

        enum VS_SETTINGS {
            COSMOS_VS_CLASS = 0x0,
            COSMOS_VS_CPU,
            COSMOS_VS_VEHICLES,
            COSMOS_VS_COURSES,
            COSMOS_VS_ITEMS,
            COSMOS_VS_RACES,
        };

        enum COSMOS_VS_SETTINGS {
            AURORA_VS_TRACK_LIST = 0x0,
            COSMOS_VS_VARIANT_SELECTION,
            COSMOS_VS_MEGA_CLOUD,
            COSMOS_VS_ALL_ITEMS_CAN_LAND,
        };

        enum AURORA_ACCESIBILITY_SETTINGS {
            AURORA_ACC_Y_TO_WHEELIE = 0x0,
            AURORA_ACC_CAMERA_FOV,
            AURORA_ACC_CAMERA_SHAKE,
            AURORA_ACC_BLOOM,
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

        enum AURORA_VS_SETTING_OPTIONS {
            TRACK_LIST_ALL = 0x0,
            TRACK_LIST_RETROS = 0x1,
            TRACK_LIST_CTS = 0x2,
        };

        enum AURORA_RANDOM_COMBO {
            RANDOM_COMBO_DISABLED = 0x0,
            RANDOM_COMBO_INDIVIDUAL = 0x1,
            RANDOM_COMBO_COMMON = 0x2,
        };

        enum AURORA_ACC_EFFECTS {
            ACC_ENABLED,
            ACC_REDUCED,
            ACC_DISABLED,

            ACC_CONTROLS_DISABLED = 0x0,
            ACC_CONTROLS_ONLY_UP = 0x1,
            ACC_CONTROLS_DIRECTIONAL = 0x2,
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
            COSMOS_SETTING_HOST_VARIANT_SELECTION = COSMOS_HOST_VARIANT_SELECTION + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_FORCE_CC = COSMOS_FORCE_CC + (COSMOS_HOST_SETTINGS_1 * 8),
            COSMOS_SETTING_RACE_COUNT = COSMOS_RACE_COUNT + (COSMOS_HOST_SETTINGS_1 * 8),


            COSMOS_SETTING_VS_CLASS = COSMOS_VS_CLASS + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_CPU = COSMOS_VS_CPU + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_VEHICLES = COSMOS_VS_VEHICLES + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_COURSES = COSMOS_VS_COURSES + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_ITEMS = COSMOS_VS_ITEMS + (COSMOS_VS_SETTINGS_1 * 8),
            COSMOS_SETTING_VS_RACES = COSMOS_VS_RACES + (COSMOS_VS_SETTINGS_1 * 8),

            AURORA_SETTING_VS_TRACK_LIST = AURORA_VS_TRACK_LIST + (COSMOS_VS_SETTINGS_2 * 8),
            COSMOS_SETTING_VS_VARIANT_SELECTION = COSMOS_VS_VARIANT_SELECTION + (COSMOS_VS_SETTINGS_2 * 8),
            COSMOS_SETTING_VS_MEGA_CLOUD = COSMOS_VS_MEGA_CLOUD + (COSMOS_VS_SETTINGS_2 * 8),
            COSMOS_SETTING_VS_ALL_ITEMS_CAN_LAND = COSMOS_VS_ALL_ITEMS_CAN_LAND + (COSMOS_VS_SETTINGS_2 * 8),

            AURORA_SETTING_HOST_TRACK_LIST = AURORA_HOST_TRACK_LIST + (COSMOS_HOST_SETTINGS_2 * 8),
            COSMOS_SETTING_HOST_MEGA_TC = COSMOS_HOST_MEGA_TC + (COSMOS_HOST_SETTINGS_2 * 8),
            COSMOS_SETTING_HOST_ALL_ITEMS_CAN_LAND = COSMOS_HOST_ALL_ITEMS_CAN_LAND + (COSMOS_HOST_SETTINGS_2 * 8),
            AURORA_SETTING_HOST_RANDOM_COMBO = AURORA_HOST_RANDOM_FORCED + (COSMOS_HOST_SETTINGS_2 * 8),

            AURORA_SETTING_ACC_Y_TO_WHEELIE = AURORA_ACC_Y_TO_WHEELIE + (AURORA_ACCESIBILITY_SETTINGS_1 * 8),
            AURORA_SETTING_ACC_CAMERA_SHAKE = AURORA_ACC_CAMERA_SHAKE + (AURORA_ACCESIBILITY_SETTINGS_1 * 8),
            AURORA_SETTING_ACC_CAMERA_FOV = AURORA_ACC_CAMERA_FOV + (AURORA_ACCESIBILITY_SETTINGS_1 * 8),
            AURORA_SETTING_ACC_BLOOM = AURORA_ACC_BLOOM + (AURORA_ACCESIBILITY_SETTINGS_1 * 8),
        };

#ifdef DEBUG_COSMOS
#define PAGE_COUNT 8
#else
#define PAGE_COUNT 7
#endif
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

        static SettingPageDefinition GlobalSettingDefinitions[PAGE_COUNT + 1] = {
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
             .settingCount = 6,
             .settings = {{.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, //OpenHost
                          {.optionCount = 2, .isBool = true, .defaultValue = DISABLED}, // HAW
                          {.optionCount = 2, .isBool = true, .defaultValue = ENABLED}, //Allow Mii Heads
                          {.optionCount = 2, .isBool = true, .defaultValue = DISABLED, .nameBmg = 0x30510, .firstDescBmg = 0x40511}, //Variant Selection
                          {.optionCount = 3, .isBool = false, .defaultValue = FORCE_NONE}, // Force CC
                          {.optionCount = 8, .isBool = false, .defaultValue = RACE_COUNT_4}}
            }, //Race count
            {
                .settingCount = 6,
                .settings = {
                    {.optionCount = 4, .isBool = false, .defaultValue = VS_CLASS_150, .nameBmg = 0xd52, .firstOptionBmg = 0xd53, .firstDescBmg = 0x0d57},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_CPU_NORMAL, .nameBmg = 0xd5c, .firstOptionBmg = 0xd5d, .firstDescBmg = 0xd61},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_VEHICLES_ALL, .nameBmg = 0xd66, .firstOptionBmg = 0xd67, .firstDescBmg = 0xd6a},
                    {.optionCount = 3, .isBool = false, .defaultValue = VS_COURSE_CHOOSE, .nameBmg = 0xd70, .firstOptionBmg = 0xd71, .firstDescBmg = 0xd74},
                    {.optionCount = 4, .isBool = false, .defaultValue = VS_ITEM_RECOMMENDED, .nameBmg = 0xd98, .firstOptionBmg = 0xd99, .firstDescBmg = 0xd9d},
                    {.optionCount = 6, .isBool = false, .defaultValue = RACE_COUNT_4, .nameBmg = 0xd7a, .firstOptionBmg = 0x30351},
                }
            },
            { // VS 2
                .settingCount = 4,
                .settings = {
                    { .optionCount = 3, .isBool = false, .defaultValue = TRACK_LIST_ALL },
                    { .optionCount = 2, .isBool = true, .defaultValue = DISABLED },
                    { .optionCount = 2, .isBool = true, .defaultValue = DISABLED },
                    { .optionCount = 2, .isBool = true, .defaultValue = DISABLED },
                }
            },
            { // Host settings 2
                .settingCount = 4,
                .settings = {
                    { .optionCount = 3, .isBool = false, .defaultValue = TRACK_LIST_ALL, .nameBmg = 0x30500, .firstOptionBmg = 0x30501, .firstDescBmg = 0x40501},
                    { .optionCount = 2, .isBool = true, .defaultValue = DISABLED, .nameBmg = 0x30520, .firstOptionBmg = BMG_ENABLED_DISABLED, .firstDescBmg = 0x40521},
                    { .optionCount = 2, .isBool = true, .defaultValue = DISABLED, .nameBmg = 0x30530, .firstOptionBmg = BMG_ENABLED_DISABLED, .firstDescBmg = 0x40531}, // ALL ITEMS
                    { .optionCount = 3, .isBool = false, .defaultValue = RANDOM_COMBO_DISABLED },
                }
            },
            { //ACCESIBILITY
                .settingCount = 4,
                .settings = {
                    { .optionCount = 3, .isBool = false, .defaultValue = ACC_CONTROLS_DISABLED},
                    { .optionCount = 3, .isBool = false, .defaultValue = ACC_ENABLED },
                    { .optionCount = 3, .isBool = false, .defaultValue = ACC_ENABLED, .firstOptionBmg = 0x30711 },
                    { .optionCount = 2, .isBool = false, .defaultValue = ACC_ENABLED, .firstOptionBmg = 0x30711 },
                },
            },
        };

#ifdef DEBUG_COSMOS
        static u8 GlobalSettingsPageOrder[PAGE_COUNT] = {COSMOS_MENU_SETTINGS_1, COSMOS_RACE_SETTINGS_1,
            COSMOS_VS_SETTINGS_1, COSMOS_VS_SETTINGS_2, COSMOS_HOST_SETTINGS_1, 
            COSMOS_HOST_SETTINGS_2, AURORA_ACCESIBILITY_SETTINGS_1, COSMOS_DEBUG_SETTINGS};
#else
        static u8 GlobalSettingsPageOrder[PAGE_COUNT] = {COSMOS_MENU_SETTINGS_1, COSMOS_RACE_SETTINGS_1,
            COSMOS_VS_SETTINGS_1, COSMOS_VS_SETTINGS_2, COSMOS_HOST_SETTINGS_1, 
            COSMOS_HOST_SETTINGS_2, AURORA_ACCESIBILITY_SETTINGS_1};
#endif

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
            static inline SettingsHolder *GetStaticInstance() { return sInstance; }

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

            inline bool IsMegaCloudEnabled() {
                return (megaCloudOffline && RaceData::GetStaticInstance()->racesScenario.settings.gamemode == MODE_VS_RACE) ||
                    (megaCloudOnline && RaceData::GetStaticInstance()->racesScenario.settings.gamemode == MODE_PRIVATE_VS);
            }

            void SetMegaCloudSetting(bool online = false) {
                megaCloudOffline = this->GetSettingValue(COSMOS_SETTING_VS_MEGA_CLOUD) == ENABLED;
                megaCloudOnline = online;
            }

            inline bool CanAllItemsLand() {
                return (allItemsCanLandOffline && RaceData::GetStaticInstance()->racesScenario.settings.gamemode == MODE_VS_RACE) ||
                    (allItemsCanLandOnline && RaceData::GetStaticInstance()->racesScenario.settings.gamemode == MODE_PRIVATE_VS);
            }

            void SetAllItemsCanLandSetting(bool online = false){
                allItemsCanLandOffline = this->GetSettingValue(COSMOS_SETTING_VS_ALL_ITEMS_CAN_LAND) == ENABLED;
                allItemsCanLandOnline = online;
            }

            inline bool CanChooseVariant() {
                return (RaceData::GetStaticInstance()->menusScenario.settings.gamemode == MODE_TIME_TRIAL) ||
                    (variantSelectionOffline && RaceData::GetStaticInstance()->menusScenario.settings.gamemode == MODE_VS_RACE) ||
                    (variantSelectionOnline && RaceData::GetStaticInstance()->menusScenario.settings.gamemode == MODE_PRIVATE_VS);
            }

            void SetChooseVariant(bool online = false){
                variantSelectionOffline = this->GetSettingValue(COSMOS_SETTING_VS_VARIANT_SELECTION) == ENABLED;
                variantSelectionOnline = online;
            }

            inline bool IsRandomComboForced(){
                return (RaceData::GetStaticInstance()->menusScenario.settings.gamemode == MODE_PRIVATE_VS) && forcedRandom;
            }

            inline bool IsRandomComboCommon(){
                return (RaceData::GetStaticInstance()->menusScenario.settings.gamemode == MODE_PRIVATE_VS) && commonRandom;
            }

            void SetRandomComboOptions(bool forced, bool common){
                forcedRandom = forced;
                commonRandom = forced && common;
            }   


            void SetAllInnerSettings(){
                this->SetAllItemsCanLandSetting();
                this->SetMegaCloudSetting();
                this->SetChooseVariant();
            }

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
            CosmosFile::FileManager* currentManager;

            bool miiHeadsEnabled;
            int currentLicense;

            bool megaCloudOffline;
            bool megaCloudOnline;
            bool allItemsCanLandOffline;
            bool allItemsCanLandOnline;
            bool variantSelectionOffline;
            bool variantSelectionOnline;

            bool forcedRandom;
            bool commonRandom;
        };
    }
}

#endif
