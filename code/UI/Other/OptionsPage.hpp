#ifndef _COSMOS_EXTRAUI_OPTIONS_
#define _COSMOS_EXTRAUI_OPTIONS_

#include <kamek.hpp>
#include <game/UI/Page/Other/Options.hpp>
#include <game/UI/MenuData/MenuData.hpp>

namespace CosmosUI
{
    class OptionsPlus : public Pages::Options
    {
        public:
            OptionsPlus() {
                this->onButtonClickHandler.ptmf = static_cast<void (Options::*)(PushButton*, u32)> (&OptionsPlus::OnPlusButtonClick);
            }
            void OnInit() override;

        private:
            void OnPlusButtonClick(PushButton * button, u32 slotId);
            PushButton settingsButton;
    };
}

#endif