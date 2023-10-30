#ifndef _MAIN_DX_
#define _MAIN_DX_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

//#define NO_FAST_MENU

namespace DX
{
    enum CUSTOM_PAGE_IDS 
    {
        SETTINGS_MAIN = 0xB7,
        RACE_SETTINGS1 = 0xB8,
        MENU_SETTINGS1 = 0xB9,
    };

    const char packFolder[20] = "/DX";
    const char ghostFolder[20] = "/DX/RKG";
    
    const char SaveFile[11] = "DXData.bin";
    const char CommonArchive[14] = "/CommonDX.szs";
    const char UIArchive[10] = "/UIDX.szs";
    const char CourseArchive[14] = "/CourseDX.szs";

    enum TT_MODE{
    DX_TT_150CC,
    DX_TT_200CC,
    };  

    static TT_MODE TTMode;
    static bool isDolphin;


    s32 Open(char *path, IOS::Mode mode);
    void SetTTCC(TT_MODE mode);
    void SetCC();


    void CreateBranch(u32 from, void * to);
    void CreateCall(u32 from, void * to);
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK);
    void Shutdown();
    void Panic(char * file, int line, char *fmt, ...);
}



#endif //_MAIN_DX_