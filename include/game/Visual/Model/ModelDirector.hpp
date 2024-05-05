#ifndef _MODEL_DIRECTOR_
#define _MODEL_DIRECTOR_
#include <kamek.hpp>
#include <core/nw4r/g3d.hpp>
#include <core/egg/3D/Scn.hpp>
#include <game/System/Archive.hpp>
#include <game/Visual/ScnManager.hpp>
#include <game/Visual/Model/ModelTransformator.hpp>
#include <game/Visual/ClipInfoMgr.hpp>

/*
Contributors:
-kHacker35000vr, Melg
*/

using namespace nw4r;

class ModelDirector{
public:
    static void BindBRRES(g3d::ResFile &file, ArchiveSource type, const char *brresName); //8055b618
    ModelDirector(u32 isTransparent, ClipInfo *clipInfo, u32 bitfield); //0x8055b298 
    ModelDirector(u32 isTransparent, u32 bitfield); //0x8055b50c
    virtual ~ModelDirector(); // 8055b5bc vtable 808b47f0
    virtual void RemoveFromScn();  //0xc  8055c9f0
    virtual void EnableDraw(bool enable); //0x10 8055cb08
    virtual void SetScale(Vec3 *scale); //0x14 8055b05c
    virtual void SetScale(float scale); //0x18 8055f894
    virtual void GetScale(Vec3 &scale) const; //0x1c 8055f848
    virtual void SetScnMdlOptions(g3d::ScnMdl *scnMdl, bool enableOptions, bool doNotCalcWorldMatVtx); //0x20 8055cd68 see OptID 
    virtual void AppendToScnManager(); //0x24 8055bed4

    void Update(u32 scnMdlIdx); //8055d238
    void Load(const char *mdlName, g3d::ResFile *brres, UnkType *r6); //8055c0e8 name without extension also calls init
    void LoadWithOption(const char *mdlName, g3d::ResFile *brres, g3d::ScnMdl::BufferOption option, UnkType *r7); //8055c378
    void LoadNoAnm(const char *mdlName, g3d::ResFile *brres, UnkType *r6); //8055c520
    void Init(ScnType type, g3d::ScnMdl::BufferOption option, u32 r6, u32 r7, u32 r8, u32 r9); //8055baa0
    void InitG3D(ScnType type, g3d::ScnMdl::BufferOption option, u32 r6 ,ScnManager *gfxMgr, u32 r8, u32 r9, u32 r10); //8055bd00
    g3d::ScnLeaf *CreateScn(ScnType type, g3d::ScnMdl::BufferOption option, u32 r6, ScnManager *gfxMgr, u32 r8, u32 r9, u8 idx); //8055bf10
    void SetSoundActor(LinkedRaceActor *actor); //8055dce4 
    void LinkAnimation(u32 id, g3d::ResFile *brres, const char *name, AnmType type, bool hasBlend, 
                const char *brasd, ArchiveSource source, u8 kartArchiveIdx); //8055ddec
    ScnManager *GetScnManager() const; //8055f2c4 often inlined
    

    u32 unk_bitfield;
    /*
    0x8 = register in the gfx manager
    */
    u32 unknown_0x8;
    g3d::ResMdlData *rawMdl; //0xC
    EGG::ScnMdlEx *curScnMdlEx; //0x10 chooses which one to use based on 1st bit of bitfield
    EGG::ScnMdlEx *scnMdlEx[2]; //0x14
    u32 unknown_0x1c[2];
    u32 isTransparent;
    ModelTransformator *modelTransformator; //0x28
    UnkType unknown_0x2c[0x3C-0x2c];
    u32 gfxMgrIdx; //0 or 1
    u8 unknown_0x40[4];
    ClipInfo *clipInfo;
}; //total size 0x4c

#endif