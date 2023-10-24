#include <Controller/MiscController.hpp>
#include <core/rvl/os/OS.hpp>

namespace DXController
{

    #define CASE_BUTTON(universal, specific) \
    case universal: return (raw & specific) != 0;

    bool isPressed(RealControllerHolder * holder, ControllerType type, u32 button)
    {
        return isPressed(holder,type,button,false);
    }

    bool isPressed(RealControllerHolder * holder, ControllerType type, u32 button, bool checkNew)
    {
        u16 raw = holder->inputStates[0].buttonRaw;
        if(checkNew) raw &= ~holder->inputStates[1].buttonRaw;

        if(type == WHEEL)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, WHEEL_A)
                CASE_BUTTON(BUTTON_B, WHEEL_B)
                CASE_BUTTON(BUTTON_1, WHEEL_1)
                CASE_BUTTON(BUTTON_2, WHEEL_1)
                CASE_BUTTON(BUTTON_MINUS, WHEEL_MINUS)
                CASE_BUTTON(BUTTON_PLUS, WHEEL_PLUS)
                CASE_BUTTON(BUTTON_HOME, WHEEL_HOME)
                CASE_BUTTON(BUTTON_DPAD_UP, WHEEL_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, WHEEL_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, WHEEL_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, WHEEL_DPAD_RIGHT)
                default:
                    return false;
            }
        }
         
        else if(type == NUNCHUCK)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, NUNCHUCK_A)
                CASE_BUTTON(BUTTON_B, NUNCHUCK_B)
                CASE_BUTTON(BUTTON_C, NUNCHUCK_C)
                CASE_BUTTON(BUTTON_Z, NUNCHUCK_Z)
                CASE_BUTTON(BUTTON_1, NUNCHUCK_1)
                CASE_BUTTON(BUTTON_2, NUNCHUCK_2)
                CASE_BUTTON(BUTTON_MINUS, NUNCHUCK_MINUS)
                CASE_BUTTON(BUTTON_PLUS, NUNCHUCK_PLUS)
                CASE_BUTTON(BUTTON_DPAD_UP, NUNCHUCK_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, NUNCHUCK_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, NUNCHUCK_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT,  BUTTON_DPAD_RIGHT)
                default:
                    return false;
            }
        }
        else if(type == CLASSIC)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, CLASSIC_A)
                CASE_BUTTON(BUTTON_B, CLASSIC_B)
                CASE_BUTTON(BUTTON_X, CLASSIC_X)
                CASE_BUTTON(BUTTON_Y, CLASSIC_Y)
                CASE_BUTTON(BUTTON_R, CLASSIC_R)
                CASE_BUTTON(BUTTON_L, CLASSIC_L)
                CASE_BUTTON(BUTTON_ZL, CLASSIC_ZL)
                CASE_BUTTON(BUTTON_ZR, CLASSIC_ZR)
                CASE_BUTTON(BUTTON_MINUS, CLASSIC_MINUS)
                CASE_BUTTON(BUTTON_HOME, CLASSIC_HOME)
                CASE_BUTTON(BUTTON_PLUS, CLASSIC_PLUS)
                CASE_BUTTON(BUTTON_DPAD_UP, CLASSIC_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, CLASSIC_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, CLASSIC_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, CLASSIC_DPAD_RIGHT)
                default:
                    return false;
            }
        }
        else if(type==GCN)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, GCN_A)
                CASE_BUTTON(BUTTON_B, GCN_B)
                CASE_BUTTON(BUTTON_X, GCN_X)
                CASE_BUTTON(BUTTON_Y, GCN_Y)
                CASE_BUTTON(BUTTON_L, GCN_L)
                CASE_BUTTON(BUTTON_R, GCN_R)
                CASE_BUTTON(BUTTON_Z, GCN_Z)
                CASE_BUTTON(BUTTON_PLUS, GCN_START)
                CASE_BUTTON(BUTTON_DPAD_UP, GCN_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, GCN_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, GCN_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, GCN_DPAD_RIGHT)
                default:
                    return false;
            }
        }
                
        return false;
    }

    bool arePressed(RealControllerHolder * holder, ControllerType type, ButtonCommon buttons)
    {
        return arePressed(holder, type, buttons, false);
    }

    bool arePressed(RealControllerHolder * holder, ControllerType type, ButtonCommon buttons, bool checkNew)
    {
        bool result = true;
        for(int i = 0; i < 32; i++)
        {
            int mask = 1 << i;
            if(buttons & mask) {
                if(!isPressed(holder, type, (ButtonCommon)mask, checkNew))
                    result = false;
            }
        }
        return result;
    }
}

#ifndef __INTELLISENSE__
// Y to Wheelie
kmBranchDefAsm(0x801afbf8, 0x801afbfc)
{
    nofralloc

    li r14, 0x880
    and r15, r14, r0
    cmpw r15, r14
    bne+ end
    sub r0, r0, r15
    li r15, 0x88
    add r0, r0, r15

    end:
    sth r0, 0(r4)

    blr
}
#endif