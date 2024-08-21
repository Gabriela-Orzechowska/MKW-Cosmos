/*
Credits:
kHacker35000vr and the Sanbo team (dolphin map, PageText)
stebler (names for some of the classes)
Melg
*/

#ifndef _PAGE_
#define _PAGE_
#include <kamek.hpp>
#include <game/Race/raceinfo.hpp>
#include <game/UI/Ctrl/UIControl.hpp>
#include <game/UI/Ctrl/MessageWindowControl.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuText.hpp>

enum PageState{
    STATE_UNKNOWN = 0,
    STATE_UNLOADED,
    STATE_LOADING,
    STATE_ACTIVATING = 3,
    STATE_FOCUSED = 4,
    STATE_DEFOCUSING = 5,
    STATE_UNLOADING
};

class Page {
public:
    Page(); //8060197c 
    virtual ~Page(); //80601a04 vtable 808ba5c0
    virtual const char* GetClassName() const; //0xC 805bb25c returns "(unknown page)"
    virtual PageId GetNextPage() const; //0x10 805bb278 returns -1 
    virtual int func_0x14(); //0x14 805bb254 returns 0, seems to be able to lead to a game reset
    virtual int func_0x18(); //0x18 805bb24c 
    virtual void ChangeMenu(MenuId menuId, u32 animDirection, float delay); //0x1c 806024b0
    virtual void func_0x20(); //0x20 80602530
    virtual void AddPageLayer(PageId id, s32 animDirection); //0x24 806025b0
    virtual void OnInit(); //0x28 80601ad8 just a blr 
    virtual void OnDispose(); //0x2c 805bb248 just a blr 
    virtual void OnActivate(); //0x30 805c9888 just a blr   
    virtual void OnDeactivate(); //0x34 805bb244 just a blr
    virtual void BeforeEntranceAnimations(); //0x38 805bb240 just a blr
    virtual void AfterEntranceAnimations(); //0x3c 805bb23c just a blr
    virtual void BeforeExitAnimations(); //0x40 805bb238 just a blr
    virtual void AfterExitAnimations(); //0x44 805bb234 just a blr
    virtual void BeforeControlUpdate(); //0x48 805bb230 just a blr
    virtual void AfterControlUpdate(); //0x4c 805bf2d8 just a blr
    virtual void OnUpdate(); //0x50 805bb22c just a blr
    virtual void OnResume(); //0x54 805bb228 just a blr
    virtual void OnMenuChange(); //0x58 805bb224 just a blr
    virtual void func_0x5C(); //0x5c 805bb220 just a blr
    //virtual void Refocus();
    virtual int GetRuntimeTypeInfo() const; //0x60 805bed68 returns 809C1d10
    void InitControlGroup(u32 controlCount); //inits the sub class and sets the count to a specified value
    void AddControl(u8 controlIdx, UIControl *control, u32 isOpaque);
    
    void SetManipulatorManager(ManipulatorManager *manager); //0x80602474, can be used with the single version too
    void EndStateAnimate(float animLength, u32 animId);
    void SetTransitionSound(u32 entranceSoundId, u32 exitSoundId); //0x80602d20
    void Setup(PageId pageId); //80601a60
    void Dispose(); //80601adc
    void Activate(); //80601aec
    void Deactivate(); //80601c08
    void UpdateControls(); //806022cc
    void AnimateControls(); //8060265c
    void StartEntrance(); //80602970 inlined
    void EndEntrance(); //806029f4
    void StartExit(); //80602b04
    void EndExit(); //80602c40 
    void PlaySound(u32 soundId, u32 r5); //80602cf8
    static Page* GetPageById(PageId type);

    inline bool isFocused(){
        return currentState == STATE_FOCUSED; 
    };

    PageId pageId;
    PageState currentState; //0x8 = focus, idk others
    bool goToNextStage; //0xc
    u8 padding[3];
    u32 animationDirection;
    float animationDelay; //0x14
    float defocusAnimDelay;
    u32 curStateDuration; //0x1C
    u32 duration; //0x20
    ControlGroup controlGroup; //0x24
    ControlsManipulatorManager *manipulatorManager; //0x38
    u32 entranceSoundId;
    u32 exitSoundId;

    class ControlAnimator : ControlGroupAction{
        virtual void Calc(UIControl *control); //806027ac vtable 808ba63c
        u32 animationDirection; //0x4
        PageState pageState; //0x8
        float animationDelay; //0xC
        float direction; //0x10 = 1/-1  if animdirection = 0/1 and state = activating or defocusing
        float directionOneTenth; //0x14
        float negatedDirection; //just direction * -1
        float delay; //0x1c
        bool hasAnimatedEnded; //gets filled by Calc
        u8 padding[4];
    }; //total size 0x24

    class OnStateChangeControlAnimator : ControlGroupAction{
        void Calc(UIControl *control) override; //808ba624 80602960 resets initial position on load/unload for example
        float direction; //0x4 = 1/-1  if animdirection = 0/1 and state = activating or defocusing
    };

    class MaxAnimDelayGetter : ControlGroupAction{
        void Calc(UIControl *control) override; //808ba630 80602920
        float delay; //filled by calc from the control
    };
    static float transitionDelay;
    
}; //Total Size 0x44
static_assert(sizeof(Page) == 0x44,"Page");

//_sinit_ at 80602ef4
namespace Pages{

class ActionLess : public Page{
    ActionLess(); //805bab40 inlined
    ~ActionLess() override; //805bab84 vtable 808b7480
    void OnInit() override; //0x28 805babec
    void OnActivate() override; //0x30 805bac40  
    void AfterControlUpdate() override; //0x4c 805bac4c, ends the page
    int GetRuntimeTypeInfo() const override; //0x60 805bb28c
    ManipulatorManager manipulatorManager;
    u8 unknown_0x54[4]; //0x54
}; //0x58

class AutoEnding : public ActionLess{ //0x4F but also 0x50 and 0xc7
public:
    AutoEnding(); //805bae98
    ~AutoEnding() override; //805baf68 vtable 808b73a8
    void OnInit() override; //0x28 805bb024
    int GetRuntimeTypeInfo() const override; //0x60 805bb26c
    virtual void ResetText(); //0x64 805bb1c0
    virtual void SetMessageWindowText(u32 bmgId, const TextInfo *text); //0x68 805bb218
    CtrlMenuPageTitleText titleText; //0x58 to 0x1CC
    BusySymbol busySymbol; //0x1CC to 0x340
    MessageWindowControl *messageWindow; //type SimpleMessageWindowControl if 0x50
    u32 unknown_0x344; //set to -1 by OnInit
}; //total size 0x348
static_assert(sizeof(AutoEnding) == 0x348, "AutoEnding");
}





#endif
