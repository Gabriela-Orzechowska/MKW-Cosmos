//Credits to Melg for SIP implementation

#include <UI/RaceBase/ControlRaceBase.hpp>

namespace DXUI
{
    CustomControlBuilder * CustomControlBuilder::sHooks = NULL;

    void CustomControlBuilder::BuildCustomRaceControl(Pages::RaceHUD * raceHugPage, u32 controlCount)
    {
        u32 customControlCount = CustomControlBuilder::Count();
        raceHugPage->InitControlGroup(controlCount + customControlCount);
        CustomControlBuilder::CreateCustomControls(raceHugPage, controlCount);
        return;
    }

    kmCall(0x808562d0, CustomControlBuilder::BuildCustomRaceControl);

} // namespace DXUI
