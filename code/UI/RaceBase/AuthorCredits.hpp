#ifndef _COSMOS_AUTHOR_CREDITS_
#define _COSMOS_AUTHOR_CREDITS_

#include <kamek.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>

#define BMGOFFSET 0x13000

namespace CosmosUI{
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