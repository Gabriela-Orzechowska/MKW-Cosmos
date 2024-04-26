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