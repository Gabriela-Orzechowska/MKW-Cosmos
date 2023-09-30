#ifndef _DX_SPEEDOMETER_
#define _DX_SPEEDOMETER_

#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/UI/MenuData/Menudata.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>

namespace DXUI
{
    class ControlRaceSpeedometer : public CtrlRaceBase
    {
    private:

        void Load(const char* variant, u8 id);

    public:

        void Init() override;
        void OnUpdate() override;
        static u32 Count();
        static void Create(Page * page, u32 index);

    };
}

#endif