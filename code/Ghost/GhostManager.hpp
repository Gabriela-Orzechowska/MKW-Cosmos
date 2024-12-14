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

#include "Race/Kart/KartMovement.hpp"
#include "types.hpp"
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
#include "Item/ItemPlayer.hpp"

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

#pragma pack(push, 1)
        struct AuroraMetadata {
#define METADATA_MAGIC 0x41524D44
            enum {
                BITFIELD_ULTRA = (1 << 0),
                BITFIELD_SHROOM = (1 << 1),
                BITFIELD_200 = (1 << 2),
            };

            u32 header;
            u32 version;
            u32 sha1[5];
            u32 bitfield;
            u32 shrooms;
            u32 crc;
        };
#pragma pack(pop)

        // HelperStruct
        struct AuroraRKG {
            RKG rkg;
            AuroraMetadata metadata;
        };

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

            bool IsFinished() { return this->isFinished; }
            void MarkFinished() { this->isFinished = true; }

            //Aurora
            void ResetMetadata();
            void SetSHA1(u32* sha1) { memcpy(&this->metadata.sha1, sha1, 20); }

            void FillMetadata();
            void AddShroom(ItemPlayer* movement);

            static void CreateAndSaveFiles(void *holder);
            static char folderPath[IPCMAXPATH];

            RKG rkg __attribute__((aligned(0x20)));
            GhostLeaderboardManager leaderboard __attribute__((aligned(0x20)));
            u32 mainGhostIndex;
            CosmosFile::FolderManager *folderManager;
            TimeEntry entry;
            u32 gameSceneFrames;
            bool wereGhostsDisabled;
            bool isFinished;
            u32 currentFileSize;

        private:
            static GhostManager *sInstance;
            GhostData *files;
            u32 courseId;
            u32 rkgCount;
            u64 ttStartTime;

            // AURORA
            u8 shroomsUsed;
            bool isAuroraGhost;
            AuroraMetadata metadata __attribute__((aligned(0x20)));
        };

    }
}

#endif
