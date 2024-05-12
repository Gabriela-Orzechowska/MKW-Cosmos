#include <kamek.hpp>
#include <game/Race/Kart/KartStatus.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <main.hpp>
#include <game/Race/RaceData.hpp>

void FastFallCalc(KartStatus& status, KartPhysics& physics)
{
    #ifndef DX_FEATURES
    if(RaceData::GetStaticInstance()->racesScenario.GetSettings().engineClass == CC_100) {
    #endif

    if(status.airtime >= 2)
    {
        if(!status.bool_0x96 || (status.airtime >= 20))
        {
            ControllerHolder * holder = status.base->GetControllerHolder();
            float y_input = holder->inputStates[0].stickY;
            if(y_input <= 0.0f) y_input = 0.0f;
            else y_input *= 2;
            physics.gravity -= y_input * 0.39f;
        }
    }

    #ifndef DX_FEATURES
    }
    #endif
        status.UpdateFromInput();
}

kmCall(0x805967a4, FastFallCalc);