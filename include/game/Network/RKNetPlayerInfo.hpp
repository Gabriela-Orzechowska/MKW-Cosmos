#ifndef _RKNETPLAYERINFO_
#define _RKNETPLAYERINFO_
#include <kamek.hpp>
#include <System/Identifiers.hpp>

struct RACEHEADER1Packet{
    u32 timer;
    u32 selectId; //0x4
    u16 team[2]; //0x8
    u16 lagFrames; //0xc
    u16 kartAndCharacter[2]; //0xe treated as a u16 ingame
    u16 countdownTime; //0x12
    u8 starCount[2]; // 0x14
    u8 trackId; //0x16
    u8 unknown_0x17;
    u8 aidsBelongToPlayer[12]; //0x18
    u8 engineClass;
    u8 unknown_0x25[3];
}; //0x28

struct RH1Data{
    u32 timer; //0
    KartId kartIds[2]; //0x4
    CharacterId charIds[2]; //0xc
    CourseId trackId; //0x14
    u32 team; 
    u32 selectId; //0x1c
    u8 aidsBelongToPlayer[12]; //0x20
    u8 engineClass; //0x2c
    u8 starCount[2];
    u8 padding;
}; //0x30

class RKNetRH1Handler{
public:
    static RKNetRH1Handler *sInstance; //809c2118
    static RKNetRH1Handler* GetStaticInstance() { return sInstance; }
    static void DestroyStaticInstance(); //80663c38
    ~RKNetRH1Handler(); //80664d9c
    void FillRH1Data(); //806651b4
    u8 unknown_0x0[0x20];
    RH1Data rh1Data[12]; //0x20
}; //total size 0x260
static_assert(sizeof(RKNetRH1Handler) == 0x260, "RKNetRH1Handler");
#endif
