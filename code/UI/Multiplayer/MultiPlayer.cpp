#include "Network/RKNetSelect.hpp"
#include "Race/Kart/KartMovement.hpp"
#include "Race/RaceData.hpp"
#include "Settings/UserData.hpp"
#include "System/Identifiers.hpp"
#include "UI/Ctrl/Manipulator.hpp"
#include "UI/Page/Menu/MultiDriftSelect.hpp"
#include "UI/Page/Page.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include <UI/Multiplayer/MultiPlayer.hpp>
#include <game/Network/RKNetController.hpp>

extern KartId kartIdByWeigth[3][12];

namespace CosmosUI
{

    kmWrite32(0x806240e0, 0x60000000);
    kmWrite32(0x806508d4, 0x60000000);
    VRPagePlus* AddVRPage()
    {
        return new(VRPagePlus);
    }

    kmCall(0x806240ec, AddVRPage);

    CharSelectPlus* AddCharPage()
    {
        return new(CharSelectPlus);
    }
    kmCall(0x80623d5c, AddCharPage);
    kmWrite32(0x80623d50, 0x60000000);

    KartSelectPlus* AddKartPage() {
        return new(KartSelectPlus);
    }
    kmCall(0x80623d74, AddKartPage);
    kmWrite32(0x80623d68, 0x60000000);

    VRPagePlus::VRPagePlus() {
        this->menuState = 0;
        this->onChangeComboButton.subject = this;
        this->onChangeComboButton.ptmf = &VRPagePlus::ChangeCombo;
        this->onRandomComboButton.subject = this;
        this->onRandomComboButton.ptmf = &VRPagePlus::RandomCombo;
    }

    void VRPagePlus::OnInit()
    {
        this->InitControlGroup(0x11);

        Pages::VR::OnInit();

        PushButton* button = &this->changeComboButton;
        this->AddControl(0x0F, button, 0);
        button->Load("button", "CosmosVRScreen", "ChangeCombo", 1, 0, false);
        button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32> * )&this->onChangeComboButton, 0);
        button->GetManipulator().SetAction(START_PRESS, &button->onClickHandlerObj, 0);

        PushButton* buttonRandom = &this->randomComboButton;
        this->AddControl(0x10, buttonRandom, 0);
        buttonRandom->Load("button", "CosmosVRScreen", "RandomCombo", 1, 0, false);
        buttonRandom->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32> * )&this->onRandomComboButton, 0);
        buttonRandom->GetManipulator().SetAction(SWITCH_PRESS, &buttonRandom->onClickHandlerObj, 0);
    }

    void UpdateOKButton(PushButton& button, const char * folderName, const char * controlName, const char * variant, u32 localPlayerCount, u8 param_5, bool inaccesable)
    {
        button.Load(folderName, "CosmosVRScreen", variant, localPlayerCount, param_5, inaccesable);
    }

    kmCall(0x8064a6e8, UpdateOKButton);
    
    void VRPagePlus::ChangeCombo(const PushButton& button, u32 slotId)
    {
        this->menuState = 1;
        this->EndStateAnimate(button.GetAnimationFrameSize(), 0);
    }

    void VRPagePlus::OnActivate(){
        Pages::VR::OnActivate();
    }

    void VRPagePlus::RandomCombo(const PushButton& button, u32 hudSlotId){
        this->menuState = 2;
        this->EndStateAnimate(button.GetAnimationFrameSize(), 0);

        u32 seed = OSGetTick();
        if(true || Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboCommon()){
            RKNetController* rknet = RKNetController::GetStaticInstance();
            RKNetControllerSub& sub = rknet->GetCurrentSub();
            seed = RKNetSELECTHandler::GetStaticInstance()->receivedPackets[sub.hostAid].selectId;
            if(sub.localAid == sub.hostAid)
                seed = RKNetSELECTHandler::GetStaticInstance()->toSendPacket.selectId;
        }
        seed = seed >> 5;
        CosmosLog("Seed used: %d\n", seed);

        //The randomizer part borrowed from Melg
        Random random(seed);
        GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
        u32 kartPos[2];
        for(int i = 0; i < context->localPlayerCount; i++){
            CharacterId charId = (CharacterId) random.NextLimited(24);
            kartPos[i] = random.NextLimited(12);
            KartId kartId = kartIdByWeigth[CharacterIDToWeightClass(charId)][kartPos[i]];

            context->characters[i] = charId;
            context->karts[i] = kartId;
            context->combos[i].selCharacter = charId;
            context->combos[i].selKart = kartId;
        }
        CharSelectPlus::GetPage()->SetRandomCharacters(context->characters[0], context->characters[1]);
        KartSelectPlus::GetPage()->SetRandomKarts(kartPos[0], kartPos[1]);
    }   

    void CharSelectPlus::BeforeControlUpdate(){
        if(!isRandom) return;

        this->controlsManipulatorManager.inaccessible = true;
        GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
        if(this->rouletteCounter > 0) {
            this->rouletteCounter--;
            for(int i = 0; i < context->localPlayerCount; i++)
            {
                CharacterId previous = this->tempCharacters[i];
                CharacterId currentSelected = previous;
                if(currentSelected == CHARACTER_NONE) currentSelected = context->characters[i];
                Random random;
                if(rouletteCounter == 0) {
                    this->ctrlMenuCharSelect.GetButtonDriver(this->actualCharacters[i])->HandleClick(i, -1);
                }
                else if(rouletteCounter % 5 == 0) {
                    if(rouletteCounter > 5)
                    {
                        do {
                            this->tempCharacters[i] = (CharacterId) random.NextLimited(24);
                        } while (previous == this->tempCharacters[i]);
                    }
                    else {
                        this->tempCharacters[i] = this->actualCharacters[i];
                    }
                    if(currentSelected != CHARACTER_NONE) this->ctrlMenuCharSelect.GetButtonDriver(currentSelected)->HandleDeselect(i, -1);
                    this->ctrlMenuCharSelect.GetButtonDriver(this->tempCharacters[i])->HandleSelect(i, -1);
                    this->ctrlMenuCharSelect.GetButtonDriver(this->tempCharacters[i])->Select(i);
                }
            }
        }
    }

    PushButton* KartSelectPlus::GetButton(u32 idx) {
        return (PushButton*) this->controlGroup.GetControl(2)->childrenGroup.GetControl(idx/2)->childrenGroup.GetControl(idx % 2);
    }


    void KartSelectPlus::BeforeControlUpdate(){
        if(!isRandom) return;

        this->controlsManipulatorManager.inaccessible = true;
        GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
        if(this->rouletteCounter > 0) {
            this->rouletteCounter--;
            u32 previous = this->tempKarts[0];
            u32 currentSelected = previous;
            if(currentSelected == KART_NONE) currentSelected = 0;
            Random random;
            if(rouletteCounter == 0) {
                this->GetButton(this->actualKarts[0])->HandleClick(0, -1);
            }
            else if(rouletteCounter % 5 == 0) {
                if(rouletteCounter > 5)
                {
                    do {
                        this->tempKarts[0] = random.NextLimited(12);
                    } while (previous == this->tempKarts[0]);
                }
                else {
                    this->tempKarts[0] = this->actualKarts[0];
                }
                if(currentSelected != KART_NONE) this->GetButton(currentSelected)->HandleDeselect(0, -1);
                this->GetButton(this->tempKarts[0])->HandleSelect(0, -1);
                this->GetButton(this->tempKarts[0])->Select(0);
            }
        }
    }

    void AddChangeComboPage(Scene& scene, PageId id)
    {
        scene.CreatePage(id);
        scene.CreatePage(CHARACTER_SELECT);
        switch (scene.menuId)
        {
            case(P1_WIFI_VS_VOTING):
            case(P1_WIFI_FRIEND_ROOM_VS_VOTING):
            case(P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
                scene.CreatePage(KART_SELECT);
                scene.CreatePage(DRIFT_SELECT);
                break;
            case(P1_WIFI_BATTLE_VOTING):
            case(P1_WIFI_FRIEND_ROOM_BALLOON_VOTING):
            case(P1_WIFI_FRIEND_ROOM_COIN_VOTING):
                scene.CreatePage(BATTLE_KART_SELECT);
                scene.CreatePage(DRIFT_SELECT);
                break;
            case(P2_WIFI_VS_VOTING):
            case(P2_WIFI_FRIEND_ROOM_VS_VOTING):
            case(P2_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
            case(P2_WIFI_BATTLE_VOTING):
            case(P2_WIFI_FRIEND_ROOM_BALLOON_VOTING):
            case(P2_WIFI_FRIEND_ROOM_COIN_VOTING):
                scene.CreatePage(MULTIPLAYER_KART_SELECT);
                scene.CreatePage(MULTIPLAYER_DRIFT_SELECT);
                break;
        }
        return;
    }
    kmCall(0x8062e09c, AddChangeComboPage);
    kmCall(0x8062e7e0, AddChangeComboPage);
    kmCall(0x8062e870, AddChangeComboPage);
    kmCall(0x8062e0e4, AddChangeComboPage);
    kmCall(0x8062e900, AddChangeComboPage);
    kmCall(0x8062e990, AddChangeComboPage);
    kmCall(0x8062e708, AddChangeComboPage);
    kmCall(0x8062e798, AddChangeComboPage);
    kmCall(0x8062ea68, AddChangeComboPage);
    kmCall(0x8062eaf8, AddChangeComboPage);
    kmCall(0x8062eb88, AddChangeComboPage);
    kmCall(0x8062ec18, AddChangeComboPage);

    void AddCharacterSelectLayer(Pages::CountDownTimer& page, PageId pageId, s32 animationDirection)
    {
        VRPagePlus* vrPage = VRPagePlus::GetPage();
        COSMOS_ASSERT_NOT_NULL(vrPage);
        if(vrPage->menuState != 0) pageId = CHARACTER_SELECT;
        return page.AddPageLayer(pageId, animationDirection);
    }

    kmCall(0x806509d0, AddCharacterSelectLayer);
    kmWrite32(0x8064a61c, 0x60000000);



}
