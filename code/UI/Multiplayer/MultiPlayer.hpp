#ifndef _COSMOS_MULTIPLAYER_UI_
#define _COSMOS_MULTIPLAYER_UI_

#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Page/Menu/CharacterSelect.hpp>
#include <game/UI/Page/Menu/KartSelect.hpp>
#include <game/UI/Page/Menu/MultiKartSelect.hpp>
#include <game/UI/Page/Menu/BattleKartSelect.hpp>
#include <game/UI/Page/Menu/DriftSelect.hpp>
#include <game/UI/Page/Menu/MultiDriftSelect.hpp>
#include <game/UI/Page/Other/VR.hpp>
#include <game/UI/Page/Other/CountDownTimer.hpp>

namespace CosmosUI
{
    class VRPagePlus : public Pages::VR
    {
        public:
            VRPagePlus();
            void OnInit() override;
            u8 menuState;
        private:
            void ChangeCombo(const PushButton& button, u32 slotId);
            PtmfHolder_2A<VRPagePlus, void, const PushButton&, u32> onChangeComboButton;
            PushButton changeComboButton;
    };
}


#endif