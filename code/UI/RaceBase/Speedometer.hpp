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
        void SetSpeedoVariant(u8 variant);

    public:

        void Init() override;
        void OnUpdate() override;
        static u32 Count();
        static void Create(Page * page, u32 index);
    private:
        s8 places[5];
        

    };
}

#endif