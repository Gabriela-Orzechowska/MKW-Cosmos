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
    static MenuData *GetStaticInstance(); //80634c90
    static void DestroyStaticInstance(); //80634cc8
    static Pages::System *CreateSystemPages(u8 idx, MenuId id); //80634a64
    
    MenuData(); //80634d40
    ~MenuData(); //80634dc4
    void Init(); //80634e44
    void StartTransition(u32 delay, u32 r5);
    void LoadNextMenu();
    Scene* curScene; /* http://wiki.tockdom.com/wiki/List_of_Identifiers#Menu_Identifiers */
    MenuId menuType[2];
    MenuId nextMenuId;
    MenuId prevMenuId;
    u32 openingTransitionAnimIdx;
    u32 transitionAnimIdx;
    u32 transitionDelay;
    bool firstLoad;
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


extern "C"{
    void SetupMenuLoad();
}
#endif