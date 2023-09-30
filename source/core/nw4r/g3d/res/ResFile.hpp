#ifndef _NW4R_G3D_RESFILE_
#define _NW4R_G3D_RESFILE_
#include "types.hpp"
#include <core/nw4r/g3d/res/ResCommon.hpp>
#include <core/nw4r/g3d/res/ResDic.hpp>
#include <core/nw4r/g3d/res/ResMdl.hpp>
#include <core/nw4r/g3d/res/ResAnmChr.hpp>
#include <core/nw4r/g3d/res/ResAnmClr.hpp>
#include <core/nw4r/g3d/res/ResAnmTexSrt.hpp>
#include <core/nw4r/g3d/res/ResAnmTexPat.hpp>
#include <core/nw4r/g3d/res/ResAnmShp.hpp>

namespace nw4r{
namespace g3d{

struct ResFileHeaderData{
    char magic[4];
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};

struct ResTopLevelDictData{
    ResBlockHeaderData header;
    ResDicData dic;
};

struct ResFileData{
    ResFileHeaderData fileHeader;
    ResTopLevelDictData dict;
};

class ResFile : public ResCommon<ResFileData>{ //BRRES
public:
    bool CheckRevision() const; //8004d900
    void Init(); //8004d630
    bool Bind(ResFile rhs); //8004d1f0
    ResMdl GetResMdl(const char *name) const; //8004c240
    ResMdl GetResMdl(int idx) const; //8004c350
    ResAnmChr GetResAnmChr(const char* str) const; //8004c780
    ResAnmClr GetResAnmClr(const char* str) const; //8004c900
    ResAnmTexSrt GetResAnmTexSrt(const char* str) const; //8004cc00
    ResAnmTexPat GetResAnmTexPat(const char* str) const; //8004ca80
    ResAnmShp GetResAnmShp(const char* str) const; //8004cd80
    u32 GetResMdlNumEntries() const; //8004c3f0
};

}//namespace g3d   
}//namespace nw4r


#endif