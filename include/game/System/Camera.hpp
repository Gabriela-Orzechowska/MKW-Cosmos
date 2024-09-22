/*
Contributors:
-Melg
*/
#ifndef _CAMERA_
#define _CAMERA_
#include <kamek.hpp>
#include <core/egg/Disposer.hpp>


enum CAMERA_FLAGS{
    FOV_LOCK = 0x4,
    IS_STATIC = 0x8,
    LOCK_VIEW_POINT = 0x10,
};

struct ShakeParams {
    float intensity;
    float multiplier;
    float lerpFactor;
};

class CameraShakeAxis {
public:
    void Update(); //805a9a40
    inline void SetTypeParams(int type, float factor); //805a99e4

    bool mIsEnabled;
    float mMultiplier;
    float mIntensity;
    float mDirection;
    float mOffset;
    ShakeParams* mShakeParams;
    float float_0x18;
};

class CameraShake {
public:
    inline CameraShake() { x.mDirection = 1.0f; y.mDirection = 1.0f; }

    inline void Update(){
        x.Update();
        y.Update();
    }
    void SetTypeParams(int type, float factor) {
        x.SetTypeParams(type, factor);
        y.SetTypeParams(type, factor);
    };

    CameraShakeAxis x;
    CameraShakeAxis y;
};

class RaceCamera{
public:
    RaceCamera(); //805a1d10
    virtual void func_vtable(); //0x808b6c40, also inherits from a bunch
    Mtx34 worldView;
    u8 unknown_0x34[0x11c-0x34];
    float currentFov;
    float additionalFov;
    u8 unknown_0x124[0x334-0x124];
    u16 flags; 
    u8 unknown_0x336[2];
    CameraShake* cameraShake;
    u8 unknown_0x33c[2];
    bool enableFovEffects; //0x33E
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


class CameraManager {
public:
    static CameraManager* sInstance;
    static CameraManager* GetStaticInstance() { return sInstance; }
    static void CreateStaticInstance();
    static void DestroyStaticInstance();

    virtual ~CameraManager();
    void ApplyShake(void* unk, u32 r5, u32 type, u32 rumble);

    EGG::TDisposer<CameraManager> disposer;
    RaceCamera** cameras;
    RaceCamera** cameras2; //?
    u32 cameraCount;
    u32 playerCount;
    bool unknown_0x24;
    bool isLiveView;
    bool hasNonPlayerCamera;
    u8 currentPlayer;
    u8 unknown_0x28[8];
    void* unknown_0x30;
    void* unknown_0x34;
};

class CameraHandler {
public:
    static CameraHandler sInstance;
    virtual void func_vtable();
    u32 hudCount;
    RaceCameraManager **cameraArray; //one per hudSlot, actually also exists outside of races
};

#endif
