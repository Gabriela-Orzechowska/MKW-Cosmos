/*
MenuData is the class that holds the information about the current displayed Menu.
Documentation:
kHacker35000vr
Melg
*/

#ifndef _MENUDATA_
#define _MENUDATA_
#include <kamek.hpp>
#include <game/UI/MenuData/Scene.hpp>
#include <game/UI/MenuData/MenuDataPad.hpp>
#include <game/UI/MenuData/SavedGhostsHandler.hpp>
#include <game/UI/MenuData/SystemBMGHolder.hpp>
#include <game/UI/MenuData/MenuData98.hpp>

typedef u32 SectionId;
class Page;

enum MenuState {
    NO_TRANSITION=0x0,
    SET_TRANSITION=0x1,
    PAGE_DISABLED=0x2,
    MENU_ENDTRANSITION=0x3,
    PAGE_ENDTRANSITION=0x4
};

class MenuData {
public:
    static MenuData *sInstance;
    static MenuData *CreateStaticInstance(); //80634c90
    static MenuData * GetStaticInstance() { return sInstance; }
    static void DestroyStaticInstance(); //80634cc8
    static Pages::System *CreateSystemPages(u8 idx, MenuId id); //80634a64
    void RequestSceneChange(u32 delay, u32 fadeOutColor); //80635ac8
    void SetNextSection(SectionId id, u32 animDirection);  //80635a3c only if new priority > old priority
    MenuData(); //80634d40
    ~MenuData(); //80634dc4
    void Init(); //80634e44
    void StartTransition(u32 delay, u32 r5);
    void LoadNextMenu();
    Scene* curScene; /* http://wiki.tockdom.com/wiki/List_of_Identifiers#Menu_Identifiers */ //0x0
    MenuId menuType[2]; //0x4
    MenuId nextMenuId; //0xC
    MenuId prevMenuId; //0x10
    u32 openingTransitionAnimIdx; //0x14
    u32 transitionAnimIdx; //0x18
    u32 transitionDelay; //0x1c
    u8 firstLoad; //0x20
	u8 unknown_0x21[3]; 

    u32 homeMenuAction;
    u32 transitionParam;
    u32 transitionFrame;
    MenuState transitionState;
    MenuDataPad pad; //0x34
    SavedGhostsHandler *savedGhostsHandler; //0x90
    SystemBMGHolder *systemBMG; //0x94
    MenuData98 *menudata98;

}; //Total Size 0x9C
size_assert(MenuData, 0x9C);


extern "C"{
    void SetupMenuLoad();
}
#endif