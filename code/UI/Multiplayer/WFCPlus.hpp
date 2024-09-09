#include <kamek.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <Aurora/AuroraAPI.hpp>

namespace Cosmos {
    namespace UI {
        class WFCMainPlus : public Pages::WFCMain {
        public:
            WFCMainPlus() {
                this->onWorldwideClick.ptmf = static_cast<void (WFCMain::*)(PushButton*,u32)>
                    (&WFCMainPlus::OnSettingsClick);
                this->onButtonSelectHandler.ptmf = static_cast<void (WFCMain::*)(PushButton*,u32)>
                    (&WFCMainPlus::OnMyButtonSelect);

            }
            void OnInit() override;
            void OnActivate() override;

            void OnSettingsClick(PushButton* button, u32 hudSlotId);
            void OnMyButtonSelect(PushButton* button, u32);

            static void GetPlayerCountCallback(void* arg, u32 val);

        };
    }
}
