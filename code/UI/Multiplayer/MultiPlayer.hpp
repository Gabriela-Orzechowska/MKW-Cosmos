#ifndef _COSMOS_MULTIPLAYER_UI_
#define _COSMOS_MULTIPLAYER_UI_

#include "System/Identifiers.hpp"
#include "UI/Ctrl/PushButton.hpp"
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

#define COUNTER_VALUE 90

const KartId AutoKarts[] = { JETSETTER, SPRINTER, BLUE_FALCON, SPEAR, NITROCYCLE,};

namespace CosmosUI
{
    class VRPagePlus : public Pages::VR
    {
        public:
            VRPagePlus();
            void OnInit() override;
            void OnActivate() override;
            u8 menuState;

            void SetRandomCombo();

            void ChangeCombo();
            void RandomCombo();


            static inline VRPagePlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<VRPagePlus>(PLAYER_LIST_VR_PAGE); }
        private:
            void ChangeComboButton(const PushButton& button, u32 hudSlotId);
            void OnSettingsButton(const PushButton& button, u32 hudSlotId);

            void OnConfirmButtonClick(u32 choice, PushButton& button);

            PtmfHolder_2A<VRPagePlus, void, const PushButton&, u32> onChangeComboButton;
            PtmfHolder_2A<VRPagePlus, void, const PushButton&, u32> onSettingsButton;
            PtmfHolder_2A<VRPagePlus, void, u32, PushButton&> onConfirmButton;
            PushButton changeComboButton;
            PushButton settingsButton;
            bool hasSetRandomCombo;
    };

    class CharSelectPlus : public Pages::CharacterSelect
    {
        public:
            static inline CharSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<CharSelectPlus>(CHARACTER_SELECT); }
            CharSelectPlus() : isRandom(false), rouletteCounter(0) {
                actualCharacters[0] = CHARACTER_NONE;
                actualCharacters[1] = CHARACTER_NONE; 
                tempCharacters[0] = CHARACTER_NONE;
                tempCharacters[1] = CHARACTER_NONE; 
            } 
            void BeforeControlUpdate() override;
            void SetRandomCharacters(CharacterId p1Char, CharacterId p2Char = CHARACTER_NONE){
                actualCharacters[0] = p1Char;
                actualCharacters[1] = p2Char;
                isRandom = true;
                rouletteCounter = COUNTER_VALUE;
            }
            void SetFastChoose() {
                rouletteCounter = 5;
            }   
        private:
            CharacterId actualCharacters[2];
            CharacterId tempCharacters[2];
            u32 rouletteCounter;
            bool isRandom;
    };

    class KartSelectPlus : public Pages::KartSelect
    {
        public:
            static inline KartSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<KartSelectPlus>(KART_SELECT); }

            KartSelectPlus() : isRandom(false), rouletteCounter(0) {
                actualKarts[0] = KART_NONE;
                actualKarts[1] = KART_NONE; 
                tempKarts[0] = KART_NONE;
                tempKarts[1] = KART_NONE; 
            } 
            void BeforeControlUpdate() override;
            PushButton* KartSelectPlus::GetButton(u32 idx);
            void SetRandomKarts(u32 p1Kart){
                actualKarts[0] = p1Kart;
                isRandom = true;
                rouletteCounter = COUNTER_VALUE;
            }
            void SetFastChoose() {
                rouletteCounter = 5;
            }   
        private:
            u32 actualKarts[2];
            u32 tempKarts[2];
            u32 rouletteCounter;
            bool isRandom;
    };
    class DriftSelectPlus : public Pages::DriftSelect {
        public:
            static inline DriftSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<DriftSelectPlus>(DRIFT_SELECT); }

            DriftSelectPlus() : isRandom(false), rouletteCounter(0) {
                isManual[0] = true;
                isManual[1] = true;
                tempOption[0] = 0;
                tempOption[1] = 0;
            };

            void BeforeControlUpdate() override;
            void SetDriftOption(bool p1 = true){
                isManual[0] = p1;
                isRandom = true;
                rouletteCounter = (COUNTER_VALUE / 2);                
            }

            void SetFastChoose(){
                rouletteCounter = 5;
            }

        private:
            bool isManual[2];
            u8 tempOption[2];
            bool isRandom;
            u32 rouletteCounter;

    };
}


#endif
