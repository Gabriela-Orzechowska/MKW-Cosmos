#pragma once
#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/RK/RKSystem.hpp>
#include <game/UI/Ctrl/PushButton.hpp>

namespace Cosmos
{
    #define CT_OFFSET 0x100
    
    #pragma pack(push, 1)
    struct Track{
        u8 slot;
        u8 musicSlot;
    };

    struct Cups{
        u32 fileSize;
        u16 cupCount;
        Track tracks[1];
    };

    #pragma pack(pop)

    class CupManager
    {
        public:
        static CupManager * sInstance;
        static CupManager * GetStaticInstance() {return sInstance; }

        CupManager();

        int GetCupCount() { return cupDef->cupCount; }
        int GetTrackID() const { return winningCourse; };

        int GetCurrentTrackSlot();
        int GetCurrentMusicSlot();

        void UpdateSelectedCourse(PushButton * button);


        u32 winningCourse;
        u32 lastSelectedCourse;
        u32 lastSelectedCup;
        u32 lastSelectedButton;

        u32 * currentLayoutArray;
        private:
        Cups * cupDef;
        
    };
} // namespace Cosmos
