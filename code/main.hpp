#ifndef _MAIN_COSMOS_
#define _MAIN_COSMOS_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

//#define NO_FAST_MENU

namespace Cosmos
{
    enum CUSTOM_PAGE_IDS 
    {
        WARNING_PAGE = 0xBA,
        SETTINGS_MAIN = 0xB7,
        RACE_SETTINGS1 = 0xB8,
        MENU_SETTINGS1 = 0xB9,
    };

    const char packFolder[20] = "DX";
    const char ghostFolder[20] = "DX/RKG";
    
    const char SaveFile[11] = "DXData.bin";
    const char CommonArchive[14] = "/CommonDX.szs";
    const char UIArchive[10] = "/UIDX.szs";
    const char CourseArchive[14] = "/CourseDX.szs";

    enum TT_MODE{
        COSMOS_TT_150cc,
        COSMOS_TT_200cc,
    };  

    static TT_MODE TTMode;
    static bool isDolphin = false;

    IOS::IPCResult Open(char *path, IOS::Mode mode);
    void SetTTCC(TT_MODE mode);
    void SetCC();

    TT_MODE GetTTMode();

    void CreateBranch(u32 from, void * to);
    void CreateCall(u32 from, void * to);
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK);
    void Shutdown();
    void Shutdown(bool force);
    void Restart();
    void Panic(char * file, int line, char *fmt, ...);
}



#endif //_MAIN_COSMOS_