#ifndef _WFCMAIN_
#define _WFCMAIN_
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
/*
Contributors:
-Melg
*/
class FriendStatusButton : public PushButton {
    //no ctor, vtable 0x808bffc0
    ~FriendStatusButton() override; //8064b2fc
    u32 frames; //increments by 1 each frame, resets at 300
    u32 unknown_0x25C; //idk
}; //total size 0x25C

namespace Pages{
class WFCMain : public Page{
public:
    WFCMain(); //8064b640
    ~WFCMain() override; //vtable 0x808bff44 8064b850
    PageId GetNextPage() const override; //0x8064d688
    void OnInit() override; //0x8064b910 
    void OnActivate() override; //0x8064bc90
    int GetRuntimeTypeInfo() const override; //0x8064d690
    void OnButtonSelect(PushButton *pushButton, u32 hudSlotId); //8064bf4c
    PtmfHolder_2A<WFCMain, void, PushButton*, u32> onWorldwideClick; //0x44 vtable 808bffb4 8064bce8
    PtmfHolder_2A<WFCMain, void, PushButton*, u32> onContinentalClick; //0x58 vtable 808bffb4 8064bdc4
    PtmfHolder_2A<WFCMain, void, PushButton*, u32> onFriendsClick; //0x6C vtable 808bffb4 8064bea0
    PtmfHolder_2A<WFCMain, void, PushButton*, u32> onBackButtonPress; //0x80 vtable 808bffb4 8064bee4 ctrlmenubackbutton
    PtmfHolder_2A<WFCMain, void, PushButton*, u32> onButtonSelectHandler; //0x94 vtable 808bffb4 8064bf4c
    PtmfHolder_1A<WFCMain, void, u32> onBackPress; //0xA8 vtable 808bffa8 8064bfb8
    CtrlMenuPageTitleText ctrlMenuPageTitleText; //0xBC
    PushButton worldwideButton; //0x230
    PushButton continentalButton; //0x484
    FriendStatusButton friendsButton; //0x6D8
    CtrlMenuBackButton backButton; //0x934
    CtrlMenuInstructionText bottomText; //0xb98
    ControlsManipulatorManager manipulatorManager; //0xd0c
    PageId nextPageId; //when you press a button
};//total size 0xf34
static_assert(sizeof(WFCMain) == 0xf34, "WFCMain");

class WFCModeSelect : public Page { //ID 0x8C
public:
    static const PageId id = WFC_VSBATTLE_SELECT;
    WFCModeSelect(); //8064bfd8
    ~WFCModeSelect() override; //8064c170 vtable 808bfec8
    PageId GetNextPage() const override; //8064d674
    void OnInit() override; //8064c220
    void OnActivate() override; //8064c4f8
    int GetRuntimeTypeInfo() const override; //8064d67c

    void OnModeButtonClick(PushButton& modeButton, u32 hudSlotId); //8064c610
    void OnModeButtonSelect(PushButton& modeButton, u32 hudSlotId); //8064c718
    void OnBackButtonClick(CtrlMenuBackButton& backButton, u32 hudSlotId); //8064c75c
    void OnBackPress(u32 hudSlotId); //8064c7b0

    PtmfHolder_2A<WFCModeSelect, void, PushButton&, u32> onModeButtonClickHandler; //0x44 8064c610
    PtmfHolder_2A<WFCModeSelect, void, PushButton&, u32> onBackButtonClickHandler; //0x58 8064c718
    PtmfHolder_2A<WFCModeSelect, void, PushButton&, u32> onButtonSelectHandler; //0x6c 8064c75c
    PtmfHolder_1A<WFCModeSelect, void, u32> onBackPressHandler; //0x80 8064c7b0
    CtrlMenuPageTitleText titleText; //0x94
    PushButton vsButton; //0x208
    PushButton battleButton; //0x45c
    CtrlMenuBackButton backButton; //0x6b0
    CtrlMenuInstructionText bottomText; //0x914
    ControlsManipulatorManager manipulatorManager; //0xa88
    PageId nextPage; //0xCAC
};//total size 0xf34
size_assert(WFCModeSelect, 0xcb0);

}//namespace Pages
#endif