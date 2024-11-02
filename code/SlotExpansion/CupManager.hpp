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

#pragma once
#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/RK/RKSystem.hpp>
#include <game/UI/Ctrl/PushButton.hpp>

namespace Cosmos
{
    #define CT_OFFSET 0x100
    #define GROUP_OFFSET 0x3000
    #define TRACK_BLOCK_COUNT 0x10

    inline bool isRTSlot(u32 slot){
        return slot < CT_OFFSET;
    }

    inline bool isCTSlot(u32 slot){
        return slot >= CT_OFFSET && slot < GROUP_OFFSET;
    }
    inline bool isGroupSlot(u32 slot){
        return slot >= GROUP_OFFSET;
    }

    #pragma pack(push, 1)
    struct Track{
        u8 slot;
        u8 musicSlot;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct LayoutDef {
        u32 cupCount;
        u32 cupOffset;
        u32 slots[];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct VariantDef {
        u16 count;
        u16 slot[8];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct CupConfig {
        enum Flags {
            SHARE_ICONS = 0x1,
            HAS_ALPHABETICAL = 0x2,
        };

        inline bool shareIcons() { return flags & SHARE_ICONS; }
        inline bool hasAlphabetical() { return flags & HAS_ALPHABETICAL; }
        inline bool hasVanilla() { return false; } // TODO IMPLEMENT

        u32 header;
        u32 fileVersion;
        u16 trackCount;
        u16 layoutCount;
        u16 variantGroupCount;
        u16 flags;
        u32 offToDefinitions;
        u32 offToVariants;
        u32 offToLayouts[1];
    };
    #pragma pack(pop)

    

    class CupManager
    {
        enum TrackSorting {
            DEFAULT,
            ALPHABETICAL,
        };


        public:
        static CupManager * GetStaticInstance() { return sInstance; }

        CupManager();

        int GetCupCount() const { return this->currentLayout->cupCount; }
        bool HasIconOffset() const { return this->currentLayout->cupOffset != -1U; }
        int GetIconOffset() const { return HasIconOffset() ? this->currentLayout->cupOffset : 0; } 
        int GetTrackCount() const { return this->currentLayout->cupCount * 4; } //TODO Make it a separate field
        int GetTrackID() const { return winningCourse; };
        u32 GetRandomVariantTrack(u32 slot) const;

        u32 GetCupID(u32 track) const { 
            for(int i = 0; i < this->currentLayout->cupCount * 4; i++){
                if(track == this->currentLayoutArray[i]) return i;
            }
            return 0;
        }

        u32 GetCupIconId(u32 track) const {
            return GetIconOffset() + GetCupID(track);
        }

        int GetCurrentTrackSlot() const;
        int GetCurrentMusicSlot() const;
        u32 GetTrackAtIndex(u32 slot) const { 
            u32 ret = this->currentLayoutArray[slot]; 
            if(isGroupSlot(ret)) ret = this->GetRandomVariantTrack(slot);
            return ret;
        }
        u32* GetCurrentTrackList() const { return currentLayoutArray; }

        int GetRandomTrack() const;
        int GetRandomSlot() const;
        u32 GetTrackBlockByIndex(int index) const { return trackBlocking[index]; }
        void AddTrackToBlocking(u32 slot) { 
            trackBlocking[currentTrackBlockIndex++]; 
            currentTrackBlockIndex = currentTrackBlockIndex % TRACK_BLOCK_COUNT;
        }

        void UpdateSelectedCourse(PushButton * button);
        void SetWinningTrack(u32 course) { winningCourse = course; }
        u16 GetWinningTrack() { return (u16) winningCourse; }
        VariantDef* GetVariantStruct(u32 slot) {
            if(isGroupSlot(slot)) slot -= GROUP_OFFSET;
            VariantDef* def = (VariantDef*) offsetFrom(this->cupConfig, this->cupConfig->offToVariants);
            return &def[slot];
        }
        
        void SetTrackLayout(TrackSorting sorting, u32 trackList = -1);
        
        bool IsInBlocking(int track) const;

        u32 winningCourse;
        u32 lastSelectedCourse;
        u32 lastSelectedCup;
        u32 lastSelectedButton;
        u32 lastSelectedGroup;

        u32* currentLayoutArray;
        u8 dontUpdateCourseSelectCourse;
        private:
        static CupManager* sInstance;
        CupConfig* cupConfig;
        LayoutDef* currentLayout;
        Track* definitions;
        u32 trackBlocking[TRACK_BLOCK_COUNT];
        u32 currentTrackBlockIndex;
        u32 currentTrackList;
        
    };
} // namespace Cosmos
