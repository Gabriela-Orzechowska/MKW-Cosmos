#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/Scene/BaseScene.hpp>

using namespace DXData;

void SetFrameRate(GameScene * scene, u8 mode)
{
    FRAME_MODE frameMode = (FRAME_MODE) SettingsHolder::GetInstance()->GetSettings()->pages[DX_RACE_SETTINGS_1].setting[DX_FRAME_MODE];

    if(frameMode == FORCE_30) mode = 1;
    else if(frameMode == FORCE_60) mode = 0;

    scene->SetFramerate(mode);
}

kmCall(0x80554270, SetFrameRate);
kmCall(0x80554abc, SetFrameRate);