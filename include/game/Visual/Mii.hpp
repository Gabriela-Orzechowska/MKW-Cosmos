#ifndef _MII_
#define _MII_
#include <kamek.hpp>
#include <game/File/RawMii.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/Visual/Model/ModelDirector.hpp>

/*
Contributors:
-Melg
*/


struct MiiTexMap{
    u8 unknown[0x24]; //texMap?
};

const u32 RFL_NAME_LEN = 10;
const u32 RFL_CREATOR_LEN = 10;

struct RFLCreateID {
    u32 miiID;
    u32 consoleID;
};

#pragma pack(push, 1)
struct RFLiCharData {
    // 0x00
    u16 padding0 : 1;
    u16 sex : 1;
    u16 birthMonth : 4;
    u16 birthDay : 5;
    u16 favoriteColor : 4;
    u16 favorite : 1;

    u16 name[RFL_NAME_LEN]; // 0x02
    u8 height; // 0x16
    u8 build; // 0x17
    RFLCreateID createID; // 0x18

    // 0x20
    u16 faceType : 3;
    u16 faceColor : 3;
    u16 faceTex : 4;
    u16 padding2 : 3;
    u16 localonly : 1;
    u16 type : 2;

    // 0x22
    u16 hairType : 7;
    u16 hairColor : 3;
    u16 hairFlip : 1;
    u16 padding3 : 5;

    // 0x24
    u16 eyebrowType : 5;
    u16 eyebrowRotate : 5;
    u16 padding4 : 6;

    // 0x26
    u16 eyebrowColor : 3;
    u16 eyebrowScale : 4;
    u16 eyebrowY : 5;
    u16 eyebrowX : 4;

    // 0x28
    u16 eyeType : 6;
    u16 eyeRotate : 5;
    u16 eyeY : 5;

    // 0x2A
    u16 eyeColor : 3;
    u16 eyeScale : 4;
    u16 eyeX : 4;
    u16 padding5 : 5;

    // 0x2C
    u16 noseType : 4;
    u16 noseScale : 4;
    u16 noseY : 5;
    u16 padding6 : 3;

    // 0x2E
    u16 mouthType : 5;
    u16 mouthColor : 2;
    u16 mouthScale : 4;
    u16 mouthY : 5;

    // 0x30
    u16 glassType : 4;
    u16 glassColor : 3;
    u16 glassScale : 4;
    u16 glassY : 5;

    // 0x32
    u16 mustacheType : 2;
    u16 beardType : 2;
    u16 beardColor : 3;
    u16 beardScale : 4;
    u16 beardY : 5;

    // 0x34
    u16 moleType : 1;
    u16 moleScale : 4;
    u16 moleY : 5;
    u16 moleX : 5;
    u16 padding8 : 1;

    u16 creatorName[RFL_CREATOR_LEN]; // at 0x36
};
#pragma pack(pop) 

struct RFLiStoreData {
    RFLiCharData data;
    u16 checkSum;
};
extern "C" {
    bool RFLSearchOfficialData(RFLCreateID* id, u32*);
    u16 RFLiCalculateCRC(const void* data, u32 size);
}

class Mii{ //has stuff like sex, type etc.. most likely
public:
    explicit Mii(u32 r4); //80525f88
    virtual ~Mii(); //80529034 vtable 808b3148
    static bool ComputeRawMii(RawMii *rawMii, MiiTexMap *data); //8052758c
    u32 r4;
    u8 unknown_0x8[0x12-0x8];
    wchar_t name[10]; //0x12 only used for other miis?
    u8 unknown_0x26[0x46-0x26];
    wchar_t name2[10]; //0x46
    u8 unknown_0x5A[0x68-0x5A];
    wchar_t name3[10]; //0x68
    u8 unknown_0x7C[0x94-0x7c];
    MiiTexMap texMap; //0x94
}; //total size 0xB8
static_assert(sizeof(Mii) == 0xb8, "Mii");

class MiiGroup{ 
public:
    MiiGroup();//805f9e78 vtable 808b9f68
    virtual ~MiiGroup(); //805f9f58 vtable 808b9f68
    void LoadMiiFromData(u8 index, MiiTexMap *data); //805fa6e0 replaces index with new mii
    void LoadMii(u8 index, Mii *mii); //805fa3fc
    void LoadDefaultMii(u8 idx, u16 inidx); //805fa748
    void Init(u32 miiCount, u32 r5, u32 r6); //805fa04c
    void AddMii(u8 index, RawMii *data); //805fa610
    void DeleteMii(u8 index); //805faa84
    Mii *GetMii(u8 idx); //805fa930
    MiiTexMap *GetMiiTexMap(u8 idx, u8 textureIdx); //805fa964
    Mii **mii; //0x4 pointer to the array of the miis loader
    MiiTexMap *textures[7]; //0x8
    u8 unknown_0x24[0x98-0x24];
}; //Total Size 0x98
static_assert(sizeof(MiiGroup) == 0x98, "MiiGroup");

class MiiHeadsModel : public ModelDirector {
public:
    MiiHeadsModel(); //807dbe0c 808d2e10
    ~MiiHeadsModel() override; //807dc860
    void *InitModel(Mii *mii, ModelDirector *driverModel, u32 r6, u32 r7, u32 r8, u32 id);
    Mii *mii;
    ModelDirector *driverModel;
    u8 unknown_0x54[0x70-0x54];
}; //total size 0x70

class MiiGXThing : public EGG::Disposer {
public:
    MiiGXThing(); //8077ef84 
    virtual ~MiiGXThing(); // 8077ee80 vtable 808d1630
    static MiiGXThing* GetStaticInstance()  { return sInstance; }
    void SetMiiRotation(int variant); //80780ab8

private:
    static MiiGXThing* sInstance;
    void* currentHeap;

};
//static_assert(sizeof(MiiGXThing) == 0x1bc, "MiiGXThing");


#endif
