#ifndef _MAIN_DX_
#define _MAIN_DX_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

#define NO_FAST_MENU

namespace DX
{
    enum CUSTOM_PAGE_IDS 
    {
        SET_PAGE_1 = 0xB7,
        SET_PAGE_2 = 0xB8,
    };

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


    void CreateBranch(u32 from, void * to);
    void CreateCall(u32 from, void * to);
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK);
}



#endif //_MAIN_DX_