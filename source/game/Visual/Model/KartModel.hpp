#ifndef _KART_MODEL_
#define _KART_MODEL_
#include <kamek.hpp>
#include <game/Race/Kart/KartBase.hpp>
#include <game/Visual/Model/ModelDirector.hpp>
#include <game/Visual/Mii.hpp>

class KartModelBones;

class KartModel{
public:
    static KartModel *sInstance;
    KartModel(); //807c7364
    virtual ~KartModel(); //807cdd08 vtable 808d2ad8
    KartBase base;
    UnkType *modelSubstruct; //0x808dbbf0 related to sound
    u8 unknown_0x10;
    u8 unknown_0x11[3];
    u32 unknown_0x14;
    u32 unknown_0x18;
    virtual int unknown_vtable(); //vtable 0x808d2ad8 //0x20
    u8 unknown_0x24[0x38-0x24];
    u32 trickType;
    u32 trickDirection;
    u8 unknown_0x40[0x68-0x40];
    void *driverModelBRRES; //0x68
    ModelDirector *driverModel; //linked to mdl0 of the brres above
    ModelDirector *driverModel_lod; //linked to mdl0_lod of the brres above
    UnkType *unknown_0x74;
    float unknown_0x78[0xA8-0x78]; //related to the character, direction they are looking at, angle, scale, etc...
    bool isCpu;
    u8 unknown_0xa9[0xB8-0xA9];
    UnkType *unknown_0xB8;
    Vec3 unknown_0xBC;
    bool isHq;
    u8 unknown_0xC9[0xCC-0xC9];
    UnkType *unknown_0xCC[2];
    u8 unknown_0xD4[2];
    bool isNotFocusedInLiveView;
    u8 unknown_0xD7[2];
    UnkType unknown_0xD8[2];
    u8 unknown_0xE0[0xfa-0xe0];
    u16 currentAnimation;
    u16 lastAnimation;
    u8 unknown_0xF8[0x100-0xf8];
    MiiHeadsModel *miiHeads; //0x807c795c
    KartModelBones *bones;
    u8 unknown_0x108[0x10c-0x108];
    char* characterName;
    u8 unknown_0x110[0x6e4-0x110];
};//total size 0x6e4

class KartModelBike : public KartModel{
public:
    KartModelBike(); //807cdccc
    ~KartModelBike() override; //807d3f58 vtable 808d2ab0
};//total size 0x6e4
#endif