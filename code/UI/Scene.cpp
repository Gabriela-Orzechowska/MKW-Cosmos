#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>
#include <core/egg/Fader.hpp>

namespace Cosmos
{
    static bool isSpedUp = false;
    u32 SetDolphinSpeed(EGG::ColorFader* fader, u32, u32, u32 ret){
        if(IOS::Dolphin::IsOpen())
        {
            if(fader->status == EGG::Fader::CAN_FADE_IN && !isSpedUp) { 
                IOS::Dolphin::SetSpeedLimit(0);
                isSpedUp = true;
            }

            else if (isSpedUp) {
                IOS::Dolphin::SetSpeedLimit(100);
                isSpedUp = false;
            }
        }

        return ret;
    }
    kmBranch(0x802153b0, SetDolphinSpeed);
} // namespace Cosmos
