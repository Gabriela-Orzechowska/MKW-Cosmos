/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
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

#pragma once
#include <kamek.hpp>
#include <core/System/RKSystem.hpp>
#include <core/System/SystemManager.hpp>
#include <Settings/UserData.hpp>
#include <UI/MenuDataPlus.hpp>
#include <UI/Settings/NewSettingsPage.hpp>

extern char * szsLanguageNames[7];

namespace Cosmos
{
    class LanguageManager {
    public:

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
        CZECH,
        HUNGARIAN,
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
        bool IsUpdateNeeded();
        void ResetStatus() { this->needsUpdate = false; }

        u32 GameToGlobal(u32 i, u32 a);
        void UpdateArchiveNames(bool isRace);
        bool isBoot;

        inline bool IsRace() { return mIsRace; };
    private:
        static LanguageManager* sInstance;
        u32 systemLanguage;
        u32 currentLanguageOption;
        u32 actualLanguage;
        u32 lastLanguage;
        bool isDefault;
        bool isKorean;
        bool needsUpdate;

        bool mIsRace;
    };

    static char* suffixes[] = {
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
        "_CZ",
        "_HU",
        "_J",
        "_K",
    };
} // namespace Cosmos
