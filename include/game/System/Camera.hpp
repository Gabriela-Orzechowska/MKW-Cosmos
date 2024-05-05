/*
Contributors:
-Melg
*/
#ifndef _CAMERA_
#define _CAMERA_
#include <kamek.hpp>

struct mystruct{
    int yeah;
};

enum CAMERA_FLAGS{
    FOV_LOCK = 0x4,
    IS_STATIC = 0x8,
    LOCK_VIEW_POINT = 0x10,
};

struct RaceCamera{
    RaceCamera(); //805a1d10
    virtual void func_vtable(); //0x808b6c40, also inherits from a bunch
    Mtx34 worldView;
    u8 unknown_0x34[0x11c-0x34];
    float currentFov;
    float additionalFov;
    u8 unknown_0x124[0x334-0x124];
    u16 flags; 
    u8 unknown_0x336[0x33E - 0x336];
    u8 enableFovEffects; //0x33E
    u8 unknown_33f[0x342-0x33f];
    u8 enableReplay;
    u8 unknown_0x342[4];
}; //total size 0x348
size_assert(RaceCamera, 0x348);

class RaceCameraManager {
public:
    RaceCameraManager(); //8056581c
    void SetProjection(u32 id, u32 r5);
    u8 unknown_0x0[0x38-0x0]; //bunch of inherited structs
    virtual void func_vtable(); //808b4c20
    u8 unknown_0x3C[0x6C-0x3C];
    RaceCamera *unkStruct;
    u8 unknown_0x70[0x10C-0x70];
}; //total size 0x10C



class CameraHandler {
public:
    static CameraHandler sInstance;
    virtual void func_vtable();
    u32 hudCount;
    RaceCameraManager **cameraArray; //one per hudSlot, actually also exists outside of races
};

#endif