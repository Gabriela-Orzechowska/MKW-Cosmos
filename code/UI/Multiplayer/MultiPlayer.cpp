#include <UI/Multiplayer/MultiPlayer.hpp>

namespace DXUI
{

    bool isVotingScreen(MenuId menuId)
    {
        return (menuId == P1_WIFI_VS_VOTING || menuId == P1_WIFI_BATTLE_VOTING 
        || menuId == P1_WIFI_FRIEND_ROOM_VS_VOTING || menuId == P1_WIFI_FRIEND_ROOM_BALLOON_VOTING 
        || menuId == P1_WIFI_FRIEND_ROOM_COIN_VOTING || menuId == P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING);
    }

    Pages::VR * AddVRPage()
    {
        MenuId menuId = MenuData::sInstance->curScene->menuId;
        if(isVotingScreen(menuId))
        {
            return new(VRPagePlus);
        }
        else
            return new(Pages::VR);
    }

    kmCall(0x806240ec, AddVRPage);

    VRPagePlus::VRPagePlus() : menuState(0) {
        this->onChangeComboButton.subject = this;
        this->onChangeComboButton.ptmf = &VRPagePlus::ChangeCombo;
    }

    void VRPagePlus::OnInit()
    {
        this->InitControlGroup(0x10);
        Pages::VR::OnInit();

        PushButton * button = &this->changeComboButton;
        this->AddControl(0x0F, button, 0);
        button->Load("button", "DXVRScreen", "ChangeCombo", 1, 0, false);
        button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32> * )&this->onChangeComboButton, 0);
        button->manipulator.SetAction(START_PRESS, &button->onClickHandlerObj, 0);
    }

    void UpdateOKButton(PushButton * button, const char * folderName, const char * controlName, const char * variant, u32 localPlayerCount, u8 param_5, bool inaccesable)
    {
        MenuId menuId = MenuData::sInstance->curScene->menuId;
        if(isVotingScreen(menuId))
        {
            controlName = "DXVRScreen";
        }
        button->Load(folderName, controlName, variant, localPlayerCount, param_5, inaccesable);
    }

    kmCall(0x8064a6e8, UpdateOKButton);

    void VRPagePlus::ChangeCombo(PushButton * button, u32 slotId)
    {
        this->menuState = 1;
        this->EndStateAnimate(button->GetAnimationFrameSize(), 0);
    }

    void AddChangeComboPage(Scene * scene, PageId id)
    {
        scene->CreatePage(id);
        scene->CreatePage(CHARACTER_SELECT);
        switch (scene->menuId)
        {
            case(P1_WIFI_VS_VOTING):
            case(P1_WIFI_FRIEND_ROOM_VS_VOTING):
            case(P1_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
                scene->CreatePage(KART_SELECT);
                scene->CreatePage(DRIFT_SELECT);
                break;
            case(P1_WIFI_BATTLE_VOTING):
            case(P1_WIFI_FRIEND_ROOM_BALLOON_VOTING):
            case(P1_WIFI_FRIEND_ROOM_COIN_VOTING):
                scene->CreatePage(BATTLE_KART_SELECT);
                scene->CreatePage(DRIFT_SELECT);
                break;
            case(P2_WIFI_VS_VOTING):
            case(P2_WIFI_FRIEND_ROOM_VS_VOTING):
            case(P2_WIFI_FRIEND_ROOM_TEAM_VS_VOTING):
            case(P2_WIFI_BATTLE_VOTING):
            case(P2_WIFI_FRIEND_ROOM_BALLOON_VOTING):
            case(P2_WIFI_FRIEND_ROOM_COIN_VOTING):
                scene->CreatePage(MULTIPLAYER_KART_SELECT);
                scene->CreatePage(MULTIPLAYER_DRIFT_SELECT);
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

    void AddCharacterSelectLayer(Pages::CountDownTimer * page, PageId pageId, s32 animationDirection)
    {
        Scene * scene = MenuData::sInstance->curScene;
        MenuId menuId = scene->menuId;
        if(isVotingScreen(menuId))
        {
            VRPagePlus * page = scene->Get<VRPagePlus>(PLAYER_LIST_VR_PAGE);
            if(page->menuState != 0) pageId = CHARACTER_SELECT;
        }
        return page->AddPageLayer(pageId, animationDirection);
    }

    kmCall(0x806509d0, AddCharacterSelectLayer);

    void DisableVotingControlLoading(Pages::VR * page, u32 controlCount)
    {
        MenuId menuId = MenuData::sInstance->curScene->menuId;
        if(isVotingScreen(menuId)) return;
        else page->InitControlGroup(controlCount);
        return;
    }
    kmCall(0x8064a61c, DisableVotingControlLoading);


}