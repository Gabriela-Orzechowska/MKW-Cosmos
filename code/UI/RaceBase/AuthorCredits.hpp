#ifndef _DX_AUTHOR_CREDITS_
#define _DX_AUTHOR_CREDITS_

#include <kamek.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>

#define BMGOFFSET 0x1000
#define LECODETRACKIDADDRESS 0x808ee450

namespace DXUI{
    class ControlAuthorCredits : public CtrlRaceWifiStartMessage
    {
    private:
        void Load();
    public:
        static u32 Count();
        static void Create(Page * page, u32 index);
    };
}

#endif