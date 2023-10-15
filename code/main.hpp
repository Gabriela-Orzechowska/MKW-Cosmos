#ifndef _MAIN_DX_
#define _MAIN_DX_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

namespace DX
{
    const char packFolder[20] = "/mkwdx";
    const char CommonArchive[14] = "/CommonDX.szs";
    const char UIArchive[10] = "/UIDX.szs";
    const char CourseArchive[14] = "/CourseDX.szs";

    enum TT_MODE{
    DX_TT_150CC,
    DX_TT_200CC,
    };  

    static TT_MODE TTMode;

    s32 Open(char *path, IOS::Mode mode);
    void SetTTCC(TT_MODE mode);
    void SetCC();


    void CreateBranch(u32 from, u32 to);
    void CreateCall(u32 from, u32 to);
}



#endif //_MAIN_DX_