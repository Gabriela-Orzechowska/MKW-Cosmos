#ifndef _COSMOS_SPEEDOMETER_
#define _COSMOS_SPEEDOMETER_

#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/UI/MenuData/Menudata.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>
#include <Settings/UserData.hpp>

namespace CosmosUI
{    
    class ControlRaceSpeedometer : public CtrlRaceBase
    {
    private:

        void Load(const char* variant, u8 id);

    public:

        void Init() override;
        void OnUpdate() override;
        void SetSpeedoVariant(u8 variant);
        static u32 Count();
        static void Create(Page * page, u32 index);
        
    private:
        s8 places[5];
    };

    static ControlRaceSpeedometer* speedos[4] = {nullptr, nullptr, nullptr, nullptr};
}

#endif