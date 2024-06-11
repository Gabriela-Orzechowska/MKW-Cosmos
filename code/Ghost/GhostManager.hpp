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

#ifndef _COSMOS_GHOST_MANAGER_
#define _COSMOS_GHOST_MANAGER_

#include <kamek.hpp>
#include <main.hpp>
#include <game/UI/Ctrl/UIControl.hpp>
#include <game/UI/Page/Other/GhostManager.hpp>
#include <FileManager/FileManager.hpp>
#include <FileManager/FolderManager.hpp>
#include <game/UI/Page/Other/TTSplits.hpp>
#include <game/Race/RaceData.hpp>
#include <game/UI/Page/Other/GhostSelect.hpp>
#include <Debug/IOSDolphin.hpp>
#include <Ghost/AntiCheat.hpp>
#include <core/rvl/DWC/GHTTP.hpp>

// Insprired by SIP implementation, should rewrite it to avoid issues, one of the first things I added

namespace Cosmos
{
    namespace Ghost
    {
#define GAMEMODES 2

        enum LEADERBOARDENTRY
        {
            ENTRY_1ST,
            ENTRY_2ND,
            ENTRY_3RD,
            ENTRY_4TH,
            ENTRY_5TH,
            ENTRY_FLAP,
        };

#pragma(pack, 1)
        class GhostTimeEntry
        {
        public:
            GhostTimeEntry() : minutes(0), seconds(0), miliseconds(0), character(MARIO), kart(STANDARD_KART_M){};
            u16 minutes;
            u8 seconds;
            u16 miliseconds;
            CharacterId character;
            KartId kart;
            u32 controllerType;
            RawMii mii;
            bool isActive;
        };
#pragma(pop)
        class GhostLeaderboardFile
        {
        public:
            GhostLeaderboardFile();
            u32 signature;
            u32 version;
            u32 trackId;
            u8 ghostStatus[GAMEMODES];
            u32 reserved[4 * 0x8];
            GhostTimeEntry entry[GAMEMODES][6]; // 5 times + flap
        };

        class GhostLeaderboardManager
        {
        public:
            GhostLeaderboardManager();
            GhostLeaderboardManager(const char *folderPath, u32 id);
            //~GhostLeaderboardManager();
            s32 GetLeaderboardPosition(const Timer &timer) const;
            void Update(s32 position, TimeEntry &entry, u32 id);
            void Save();
            void Save(const char *folderPath);

            void GhostTimeEntryToTimer(Timer &timer, u32 index) const;
            void GhostTimeEntryToTimeEntry(TimeEntry &entry, u32 index);

            static void CreateFile(u32 id);

        private:
            GhostLeaderboardFile file __attribute__((aligned(0x20)));
            char folderPath[IPCMAXPATH];
        };

        class GhostManager
        {
        public:
            GhostManager();
            ~GhostManager();
            static GhostManager *GetStaticInstance() { return sInstance; }
            static GhostManager *CreateStaticInstance();
            static void DestroyStaticInstance();
            void Reset();
            void Init(u32 courseId);
            bool EnableGhost(const GhostListEntry &entry);
            GhostLeaderboardManager &GetLeaderboard() { return this->leaderboard; }
            const GhostData *GetGhostData(u32 idx) const
            {
                return &this->files[idx];
            }
            bool LoadGhost(RKG *rkg, u32 index);
            void LoadGhostReplay(RKG *rkg, bool isGhostRace);
            void VerifyTime();

            static void CreateAndSaveFiles(void *holder);
            static char folderPath[IPCMAXPATH];

            RKG rkg __attribute__((aligned(0x20)));
            GhostLeaderboardManager leaderboard __attribute__((aligned(0x20)));
            u32 mainGhostIndex;
            CosmosFile::FolderManager *folderManager;
            TimeEntry entry;
            u32 gameSceneFrames;
            bool wereGhostsDisabled;

        private:
            static GhostManager *sInstance;
            GhostData *files;
            u32 courseId;
            u32 rkgCount;
            u64 ttStartTime;
        };

        const char ghostUploadLink[] = "http://cosmos.gabriela-orzechowska.com/andromeda/api/upload-rkg/%s?name=%s&time=%01dm%02ds%03d&mode=%s";

        class GhostLeaderboardAPI {
        public:
            static s32 SendGhostData(RKG* buffer, u32 bufferSize, char* sha1);
        private:
            static void SendGhostDataCallback(const char* buffer, u32 size, s32 ret, void* param);
            static bool sendRequest;
        };
    }
}

#endif
