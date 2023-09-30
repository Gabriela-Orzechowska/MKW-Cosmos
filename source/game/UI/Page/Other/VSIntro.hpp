#ifndef _VSRACEINTRO_
#define _VSRACEINTRO_
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
namespace Pages{
class VSRaceIntro : public Page { //0x8
    VSRaceIntro(); //80624690
    ~VSRaceIntro() override; //80855644 vtable 808da568
    void OnInit() override; //80855200
    int GetRuntimeTypeInfo() const override; //808556c4
    LayoutUIControl trackName; //0x44
    LayoutUIControl titleText; //0x1b8 cup_name brlyt, "VS Race - 1 of 16"
    ManipulatorManager manipulatorManager; //0x32c
}; //0x33c
static_assert(sizeof(VSRaceIntro) == 0x33c, "VSRaceIntro");
}//namespace Pages
#endif