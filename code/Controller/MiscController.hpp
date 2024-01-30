#include <kamek.hpp>
#include <game/Input/InputData.hpp>
#include <game/UI/MenuData/MenuData.hpp>

namespace CosmosController
{
    enum ButtonCommon{
        BUTTON_NONE             = 0,
        BUTTON_DPAD_LEFT        = 1 << 0,
        BUTTON_DPAD_RIGHT       = 1 << 1,
        BUTTON_DPAD_DOWN        = 1 << 2,
        BUTTON_DPAD_UP          = 1 << 3,
        BUTTON_A                = 1 << 4,
        BUTTON_B                = 1 << 5,
        BUTTON_X                = 1 << 6,
        BUTTON_Y                = 1 << 7,
        BUTTON_MINUS            = 1 << 8,
        BUTTON_PLUS             = 1 << 9,
        BUTTON_HOME             = 1 << 10,
        BUTTON_1                = 1 << 11,
        BUTTON_2                = 1 << 12,
        BUTTON_L                = 1 << 13,
        BUTTON_R                = 1 << 14,
        BUTTON_C                = 1 << 15,
        BUTTON_Z                = 1 << 16,
        BUTTON_ZL               = 1 << 17,
        BUTTON_ZR               = 1 << 18,
    };

    bool isPressed(RealControllerHolder * holder, ControllerType type, u32 button, bool checkNew);
    bool isPressed(RealControllerHolder * holder, ControllerType type, u32 button);

    bool arePressed(RealControllerHolder * holder, ControllerType type, ButtonCommon buttons, bool checkNew);
    bool arePressed(RealControllerHolder * holder, ControllerType type, ButtonCommon buttons);
}