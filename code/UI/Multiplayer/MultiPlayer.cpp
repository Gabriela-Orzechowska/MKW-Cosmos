/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Network/RKNetSelect.hpp"
#include "Race/Kart/KartMovement.hpp"
#include "Race/RaceData.hpp"
#include "Settings/UserData.hpp"
#include "System/Identifiers.hpp"
#include "UI/Ctrl/Manipulator.hpp"
#include "UI/Ctrl/PushButton.hpp"
#include "UI/Page/Menu/MultiDriftSelect.hpp"
#include "UI/Page/Other/CountDownTimer.hpp"
#include "UI/Page/Other/YesNo.hpp"
#include "UI/Page/Page.hpp"
#include "UI/Settings/NewSettingsPage.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include "main.hpp"
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

    DriftSelectPlus* AddDriftPage(){
        return new(DriftSelectPlus);
    }
    kmCall(0x80623d8c, AddDriftPage);
    kmWrite32(0x80623d80, 0x60000000);

    VRPagePlus::VRPagePlus() : menuState(0), hasSetRandomCombo(false) {
        this->onChangeComboButton.subject = this;
        this->onChangeComboButton.ptmf = &VRPagePlus::ChangeComboButton;

        this->onSettingsButton.subject = this;
        this->onSettingsButton.ptmf = &VRPagePlus::OnSettingsButton;

        this->onConfirmButton.subject = this;
        this->onConfirmButton.ptmf = &VRPagePlus::OnConfirmButtonClick;
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

        PushButton* buttonSettings = &this->settingsButton;
        this->AddControl(0x10, buttonSettings, 0);
        buttonSettings->Load("button", "CosmosVRScreen", "RandomCombo", 1, 0, false);
        buttonSettings->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32> * )&this->onSettingsButton, 0);
    }

    void UpdateOKButton(PushButton& button, const char * folderName, const char * controlName, const char * variant, u32 localPlayerCount, u8 param_5, bool inaccesable)
    {
        button.Load(folderName, "CosmosVRScreen", variant, localPlayerCount, param_5, inaccesable);
    }

    kmCall(0x8064a6e8, UpdateOKButton);
    
    void VRPagePlus::OnConfirmButtonClick(u32 choice, PushButton& button) {
        Pages::YesNoPopUp* popUp = Pages::YesNoPopUp::GetPage();    
        popUp->EndStateAnimate(0.0f, 0);

        if(choice == 0) this->ChangeCombo();
        else this->RandomCombo();
    }

    void VRPagePlus::ChangeComboButton(const PushButton& button, u32 hudSlotId){
        Pages::YesNoPopUp* popUp = Pages::YesNoPopUp::GetPage();    
        popUp->SetMessageBoxMsg(0x2831, nullptr); 
        popUp->SetupButton(0, 0xd71, nullptr, -1, &this->onConfirmButton);
        popUp->SetupButton(1, 0xd72, nullptr, -1, &this->onConfirmButton);

        this->AddPageLayer(VOTERANDOM_MESSAGE_BOX, 0);
    }

    void VRPagePlus::SetRandomCombo(){
        if(hasSetRandomCombo) return;

        u32 seed = OSGetTick();
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboCommon()){
            RKNetController* rknet = RKNetController::GetStaticInstance();
            RKNetControllerSub& sub = rknet->GetCurrentSub();
            seed = RKNetSELECTHandler::GetStaticInstance()->receivedPackets[sub.hostAid].selectId;
            if(sub.localAid == sub.hostAid)
                seed = RKNetSELECTHandler::GetStaticInstance()->toSendPacket.selectId;
        }
        seed = seed >> 5;

        //The randomizer part borrowed from Melg
        Random random(seed);
        GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
        u32 kartPos[2];
        bool manualMode[2] = { true, true };
        for(int i = 0; i < context->localPlayerCount; i++){
            CharacterId charId = (CharacterId) random.NextLimited(24);
            kartPos[i] = random.NextLimited(12);
            KartId kartId = kartIdByWeigth[CharacterIDToWeightClass(charId)][kartPos[i]];

            context->characters[i] = charId;
            context->karts[i] = kartId;
            context->combos[i].selCharacter = charId;
            context->combos[i].selKart = kartId;

            for(int j = 0; j < 5; j++){
                if(kartId == AutoKarts[j]){
                    manualMode[i] = random.NextLimited(2) == 0;
                }
            }
        }
        
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboCommon() && context->localPlayerCount > 1){
            context->characters[1] = context->characters[0];
            context->karts[1] = context->karts[0];
            context->combos[1].selCharacter = context->combos[0].selCharacter;
            context->combos[1].selKart = context->combos[0].selKart;
            kartPos[1] = kartPos[0];
            manualMode[1] = manualMode[0];
        }
        else {
            KartSelectPlus::GetPage()->SetRandomKarts(kartPos[0]);
            DriftSelectPlus::GetPage()->SetDriftOption(manualMode[0]);
        }
        CharSelectPlus::GetPage()->SetRandomCharacters(context->characters[0], context->characters[1]);

        this->hasSetRandomCombo = true;
    }

    void VRPagePlus::ChangeCombo()
    {
        this->menuState = 1;
        this->EndStateAnimate(0.0f, 0);
    }

    void VRPagePlus::OnActivate(){
        Pages::VR::OnActivate();
        Pages::CountDownTimer::GetPage()->status = STATUS_VR_PAGE;
        this->menuState = 0;
        
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboForced()){
            this->changeComboButton.isHidden = true;
            this->changeComboButton.SetPlayerBitfield(0);
            this->SetRandomCombo();
        }
    }

    void VRPagePlus::OnSettingsButton(const PushButton& button, u32) {
        CosmosUI::NewSettings* settings = CosmosUI::NewSettings::GetPage();
        PageId timerEnd = COURSESTAGE_VOTES;
        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboForced()) timerEnd = CHARACTER_SELECT;
        settings->SetPreviousPage(PLAYER_LIST_VR_PAGE, P1_WIFI_VS_VOTING, timerEnd);
        settings->SetTimer(this->timer);
        this->menuState = 3;
        this->EndStateAnimate(0.0f, 0);
    }

    void VRPagePlus::RandomCombo(){
        this->menuState = 2;
        this->EndStateAnimate(0.0f, 0);

        this->SetRandomCombo();
    }   

    void CharSelectPlus::BeforeControlUpdate(){
        Pages::CountDownTimer* timer = Pages::CountDownTimer::GetPage();
        if(timer){
            timer->status = STATUS_CUP_SELECT;
        }
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

    void DriftSelectPlus::BeforeControlUpdate(){
        if(!isRandom) return;
        this->controlsManipulatorManager.inaccessible = true;
        GlobalContext* context = MenuData::GetStaticInstance()->GetCurrentContext();
        if(this->rouletteCounter > 0) {
            --this->rouletteCounter;
            if(rouletteCounter == 0){
               this->externControls[(int)!this->isManual[0]]->HandleClick(0, -1); 
            }
            else if(rouletteCounter % 5 == 0) {
                if(rouletteCounter > 5)
                {
                    this->tempOption[0] = !this->tempOption[0];
                }
                else {
                    this->tempOption[0] = this->isManual[0];
                }
                this->externControls[(int)!this->tempOption[0]]->HandleSelect(0, -1); 
                this->externControls[(int)!this->tempOption[0]]->Select(0); 
            }
        }
    }

    void AddChangeComboPage(Scene& scene, PageId id)
    {
        scene.CreatePage(id);
        scene.CreatePage(CHARACTER_SELECT);
        scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
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
        if(!page.countdown.isActive || vrPage->menuState > 5) {
            if(vrPage->menuState < 6 && Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboForced()){
                CharSelectPlus::GetPage()->SetFastChoose();
                KartSelectPlus* kartPage = KartSelectPlus::GetPage();
                DriftSelectPlus* driftPage = DriftSelectPlus::GetPage();
                if(kartPage)
                    kartPage->SetFastChoose();
                if(driftPage)
                    driftPage->SetFastChoose();
                pageId = CHARACTER_SELECT;
            }
            else {
                pageId = COURSESTAGE_VOTES;
                page.status = STATUS_VOTES_PAGE;
            }
            vrPage->menuState = 7;
            page.timerControl.Reset();
        }
        else if(vrPage->menuState == 3) pageId = (PageId)Cosmos::SETTINGS_MAIN;
        else if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsRandomComboForced()){
            pageId = CHARACTER_SELECT;
        }
        else if(vrPage->menuState != 0) {
            pageId = CHARACTER_SELECT;
        }
        if(vrPage->menuState != 3){
            vrPage->menuState = 6;
        }

        return page.AddPageLayer(pageId, animationDirection);
    }
    kmCall(0x806509d0, AddCharacterSelectLayer);

    void ForceCharacterPage(Pages::CountDownTimer& page, PageId pageId, s32 animationDirection){
        page.AddPageLayer(pageId, animationDirection);
    }
    kmCall(0x80650a00, ForceCharacterPage);

    kmWrite32(0x80650978, 0x60000000);
    kmWrite32(0x8064a61c, 0x60000000);
}
