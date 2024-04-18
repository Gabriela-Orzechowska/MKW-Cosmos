#pragma once
#include <game/UI/Page/RaceMenu/TTPause.hpp>

namespace CosmosUI {

    static u32 buttonArray[6] = {
        Pages::RaceMenu::ButtonContinue,
        Pages::RaceMenu::ButtonRestart1,
        Pages::RaceMenu::ButtonChangeCourse,
        Pages::RaceMenu::ButtonChangeCharacter,
        37, //TTPausePlus::ButtonSettings, 
        Pages::RaceMenu::ButtonQuit,
    };


    class TTPausePlus : public Pages::TTPause {
    public:
        TTPausePlus() {};
        enum CustomButtons {
            ButtonSettings = 37,
        };

        char* GetButtonsBRCTRName() const override { return "CosmosPauseMenuTimeAttack"; }
        u32 *GetVariantsIdxArray() const override { return buttonArray; }
        u32 GetButtonCount() const override { return 6; }
    };


}