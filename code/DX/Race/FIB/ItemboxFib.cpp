#include <kamek.hpp>
#include <game/Item/Obj/FIB.hpp>
#include <game/Visual/ScnManager.hpp>
#include <game/Visual/Model/ModelDirector.hpp>
#include <game/System/Archive.hpp>

#ifdef DX_FEATURES

namespace DX
{
    static ArchiveSource fibSource = ARCHIVE_HOLDER_COURSE;

    void InitItemboxFIB(ItemObjFIB * fib)
    {
        
        fibSource = ARCHIVE_HOLDER_COURSE;
        g3d::ResFile * file = (g3d::ResFile *) ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_COURSE, "itembox.brres", 0x0);
        if(file == nullptr) 
        {
            fibSource = ARCHIVE_HOLDER_COMMON;
            file = (g3d::ResFile *) ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_COMMON, "itembox.brres", 0x0);
        }

        char animName[] = "itembox";
        //ItemObjBase::AnmParam param[5];
        /*
        u32 index = 0;
        if(file->GetResAnmChr(animName) != nullptr) 
        {
            param[index].name = animName;
            param[index].type = ANMTYPE_CHR;
            index++;
        }
        if(file->GetResAnmClr(animName) != nullptr) 
        {
            param[index].name = animName;
            param[index].type = ANMTYPE_CLR;
            index++;
        }
        if(file->GetResAnmTexPat(animName) != nullptr) 
        {
            param[index].name = animName;
            param[index].type = ANMTYPE_TEXPAT;
            index++;
        }
        if(file->GetResAnmTexSrt(animName) != nullptr) 
        {
            param[index].name = animName;
            param[index].type = ANMTYPE_TEXSRT;
            index++;
        }
        */
        fib->isLocalMultiplayer = ScnManager::pageCount > 1;
        fib->LoadGraphics("itembox.brres", "itembox", "koura_s", 0x0, NULL, (nw4r::g3d::ScnMdl::BufferOption) 0x10b, 0, 0);
        return;
    }
    kmWritePointer(0x808d1d6c, InitItemboxFIB); //807a7ac0 override

    kmWrite32(0x807a7b68,0x428001e0);
    kmWrite32(0x807a7f3c, 0x42800098);
    kmWrite32(0x807a7d90, 0x4e800020);

    void LoadProperBrres(g3d::ResFile &file, ArchiveSource type, const char *brresName)
    {
        if(brresName == "itembox.brres" && type == ARCHIVE_HOLDER_COMMON)
            type = fibSource;

        ModelDirector::BindBRRES(file, type, brresName);
        return;
    }
    kmCall(0x807a0160, LoadProperBrres);


} // namespace DX

#endif
