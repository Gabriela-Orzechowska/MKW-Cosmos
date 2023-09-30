#ifndef _SCENE_
#define _SCENE_
#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <game/UI/Layout/Layout.hpp>
#include <game/UI/Movie.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Page/System/RecognizePad.hpp>
#include <game/UI/Page/System/DpdPointer.hpp>
#include <game/UI/FriendList.hpp>

enum MenuCategory {
    CATEGORY_ACTION=0x0,
    CATEGORY_MENU=0x1,
    CATEGORY_GAMEPLAY=0x2,
    CATEGORY_TEST=0x3,
    CATEGORY_ONLINE_MENU=0x4,
    CATEGORY_UNK5=0x5,
    CATEGORY_UNK6=0x6,
    CATEGORY_UNK7=0x7,
    CATEGORY_UNK8=0x8,
    CATEGORY_UNK9=0x9,
    CATEGORY_UNK10=0xa,
    CATEGORY_UNK11=0xb
};

class Scene {
public:
    static Page *CreatePageById(PageId pageId); //0x80623390
    static MenuCategory GetType(MenuId id); //80631588
    static u32 GetSoundid(MenuId menuId); //80631c68

    template<class T>
    T* Get(PageId id) const {return (T*) this->pages[id];}
    void Set(Page *t, PageId id) {this->pages[id] = t;}
    Scene(); //0x80621d0c
    ~Scene(); //0x80621d84
    void Init(MenuId menuId); //0x80621e00
    void BuildPages(MenuId menuId); //0x8062c3a4
    void CreatePage(PageId pageId); //0x80622d08
    void CreateMoviePlayer(); //80622c68
    void CreateFriendList(); //80622cb8
    
    
    MenuId menuId;
    u32 openingAnimIdx;
    Page *pages[0xD2]; //https://wiki.tockdom.com/wiki/List_of_Identifiers#PAGE_Identifiers
    Page *activePages[11]; //0x354
    u32 layerCount; //0x37C
    Pages::RecognizePad *recognizePadPage; //0x380
    Pages::DpdPointer *pointerPage; //0x384
    bool hidepointer;
    bool pauseGame;
    bool controllerDcPause;
    bool isPaused;
	u8 unknown_0x38C[0x390 - 0x38C];

	nw4r::lyt::DrawInfo drawInfo; //0x390
    EGG::Allocator *layoutAllocator; //0x3e4
    LayoutResourceAccessorList *resourceAccessorList; //0x3E8
    PictureLayoutList *pictureLayoutList; //0x3EC every time a picturelayout is created, the game checks if the brlyt isn't already loaded
    Vec2 pointerTranslationBoxSize; //0x3f0
    Vec2 pointerScale; //0x3f8
    MoviePlayer *moviePlayer; //0x400
    FriendList *friendList; //0x404

};//Total Size 0x408
static_assert(sizeof(Scene) == 0x408, "Scene");

#endif