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

    #pragma pack(push, 1)
    struct Track{
        u8 slot;
        u8 musicSlot;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct LayoutDef {
        u32 cupCount;
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
        enum TrackLayout {
            DEFAULT,
            ALPHABETICAL,
        };


        public:
        static CupManager * GetStaticInstance() { return sInstance; }

        CupManager();

        int GetCupCount() const { return this->currentLayout->cupCount; }
        int GetTrackCount() const { return this->currentLayout->cupCount * 4; } //TODO Make it a separate field
        int GetTrackID() const { return winningCourse; };
        u32 GetRandomVariantTrack(u32 slot);

        u32 GetCupID(u32 track) const { 
            for(int i = 0; i < this->currentLayout->cupCount * 4; i++){
                if(track == this->currentLayoutArray[i]) return i;
            }
            return 0;
        }

        int GetCurrentTrackSlot() const;
        int GetCurrentMusicSlot() const;

        int GetRandomTrack() const;
        u32 GetTrackBlockByIndex(int index) const { return trackBlocking[index]; }
        void AddTrackToBlocking(u32 slot) { 
            trackBlocking[currentTrackBlockIndex++]; 
            currentTrackBlockIndex = currentTrackBlockIndex % TRACK_BLOCK_COUNT;
        }

        void UpdateSelectedCourse(PushButton * button);
        void SetWinningTrack(u32 course) { winningCourse = course; }
        u16 GetWinningTrack() { return (u16) winningCourse; }
        
        void SetTrackLayout(TrackLayout layout);
        
        bool IsInBlocking(int track) const;

        u32 winningCourse;
        u32 lastSelectedCourse;
        u32 lastSelectedCup;
        u32 lastSelectedButton;


        u32 * currentLayoutArray;
        u8 dontUpdateCourseSelectCourse;
        private:
        static CupManager * sInstance;
        CupConfig * cupConfig;
        LayoutDef* currentLayout;
        Track* definitions;
        u32 trackBlocking[TRACK_BLOCK_COUNT];
        u32 currentTrackBlockIndex;
        
    };
} // namespace Cosmos
