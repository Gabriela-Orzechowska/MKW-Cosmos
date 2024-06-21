#ifndef _RKSCENEMANAGER_
#define _RKSCENEMANAGER_
#include <types.hpp>
#include <core/egg/Scene.hpp>

using namespace EGG;

class ExpHeapGroup {
public:
    enum GroupId {
        DEFAULT = 0x0,
        RACE,
        GFX,
        KART,
        GEO_OBJ,
        COURSE,
        UI,
        EFFECT,
        SOUND,
        RESOURCE,
        HOME_MENU,
        ITEM,
        NET,
    };
    void SetHeapsGroupId(GroupId groupId); //80009bc0
    ExpHeap *heaps[3];
    ExpHeap::GroupSizeHolder groupSizes[3]; //0xC, one per heap
}; //0xc0c

class RKScene : public EggScene {
public:
    ~RKScene() override; //800073f0 vtable 802585d8
    ExpHeapGroup expHeapGroup; //0x30
    u8 unknown_0xc3c[0xc70 - 0xc3c]; //0xc3c
}; //0xc70

class RKSceneCreator : public SceneCreator { //Creates StrapScene
    EggScene *create(u32 sceneId) override; //8000a550 vtable 80270cd0
    void destroy(u32 sceneId) override;  //8000a5a8
};

class RKSceneManager : public EggSceneManager {
public:
    void calc() override;             //0x8  80009984 vtable 80270c68
    void draw() override;             //0xc  80009988
    void calcCurrentFader() override; //0x14 8000984c
    void calcCurrentFaderGL();
    virtual void calcCurrentFaderImpl(); //0x24 8000999c non-official
    virtual void drawCurrentFaderImpl(); //0x28 8000998c
    bool ChangeSceneByCreatorAfterFadeOut(u32 sceneId, RKSceneCreator *creator); //80009844, non-official

};
#endif