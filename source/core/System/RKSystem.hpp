#ifndef _RKSYSTEM_
#define _RKSYSTEM_
#include "types.hpp"
#include <core/egg/Heap.hpp>
#include <core/egg/Audio.hpp>

class GXRModeObj;
class SceneManager;

struct Video{
    GXRModeObj *mode;
    u32 unknown[2];
};//total size 0xc

class RKSystem{
public:
    static RKSystem mInstance; //802a4080
    static RKSystem *sInstance; //80385fc8
    RKSystem(); //not a true ctor 800099cc
    virtual void func_vtable(); //80270bf0
    virtual Video *GetVideo(); //0x8
    virtual EGG::Heap *GetHeapSystem2(); //0xC
    virtual void *GetDisplay(); //0x10
    virtual void *GetXFBManager(); //0x14
    virtual void *GetProcessMeter(); //0x18
    virtual SceneManager *GetSceneManager(); //0x1C
    virtual EGG::ExpAudioMgr *GetAudioManager(); //0x20
    void *MEM1ArenaLo;
    void *MEM1ArenaHi;
    void *MEM2ArenaLo;
    void *MEM2ArenaHi;    
    u32 memorySize; //0x14
    EGG::ExpHeap *EGGRootMEM1; //0x18
    EGG::ExpHeap *EGGRootMEM2;  //0x1C
    EGG::ExpHeap *EGGRootDebug;
    EGG::ExpHeap *EGGSystem;  //0x24
    void *heapSystem; //thread
    u32 unknown_0x2C; //just the start of mem1?
    u32 unknown_0x30; //idk
    u32 sysHeapSize;
    u32 gxFifoBufSize;
    GXRModeObj *mode;
    EGG::ExpAudioMgr *audioManager; //0x40
    Video *video; //0x44
    void *xfbManager; //0x48
    void *asyncDisplay; //0x4c
    void *processMeter; //0x50
    SceneManager *sceneManager; //0x54
    EGG::ExpHeap *kpadWorkHeap;
    u32 unknown_0x5c;
    EGG::ExpHeap *relLinkHeap;
    EGG::ExpHeap *heapSystem2; //the one I use 0x64
    u8 frameClock;
    u8 unknown_0x68;
    bool drawFrame;
    u8 unknown_0x6b[0x74-0x6b];
}; //total size 0x74
#endif