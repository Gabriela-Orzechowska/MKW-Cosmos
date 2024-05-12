#pragma once
#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/RK/RKSystem.hpp>
#include <game/UI/Ctrl/PushButton.hpp>

namespace Cosmos
{
    #define CT_OFFSET 0x100
    #define TRACK_BLOCK_COUNT 0x10

    #pragma pack(push, 1)
    struct Track{
        u8 slot;
        u8 musicSlot;
    };
    #pragma pack(pop)


    #pragma pack(push, 1)
    struct CupConfig {
        u32 header;
        u32 fileSize;
        u16 cupCount;
        u16 layoutCount;
        u32 offToDefinitions;
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

        int GetCupCount() const { return cupConfig->cupCount; }
        int GetTrackCount() const { return cupConfig->cupCount * 4; } //TODO Make it a separate field
        int GetTrackID() const { return winningCourse; };

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
        u32* layouts[2];
        Track* definitions;
        u32 trackBlocking[TRACK_BLOCK_COUNT];
        u32 currentTrackBlockIndex;
        
    };
} // namespace Cosmos
