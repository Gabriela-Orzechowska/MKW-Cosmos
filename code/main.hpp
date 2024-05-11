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
    };

    const char packFolder[] = "/Cosmos";
    const char ghostFolder[] = "/Cosmos/RKG";
    
    const char SaveFile[] = "CosmosData.bin";
    const char CommonArchive[] = "/CommonCosmos.szs";
    const char UIArchive[] = "/UICosmos.szs";
    const char CourseArchive[] = "/CourseCosmos.szs";

    enum TT_MODE{
        COSMOS_TT_150cc,
        COSMOS_TT_200cc,
    };  

    class System{
        public:
        static void CreateStaticInstance();
        static System * GetStaticInstance() { return sInstance; }

        void Init();
        void CreateFolders();
        TT_MODE GetTTMode() { return this->currentTTMode; }
        void SetTTMode(TT_MODE mode);

        static void Shutdown();
        static void Shutdown(bool force);
        static void Restart();
        static void HardRestart();

        private:
        static System * sInstance;
        TT_MODE currentTTMode;
    };

    IOS::IPCResult Open(char *path, IOS::Mode mode);
    void SetTTCC(TT_MODE mode);
    void SetCC();


    void CreateBranch(u32 from, void * to);
    void CreateCall(u32 from, void * to);
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK);
    void Panic(char * file, int line, char *fmt, ...);
}



#endif //_MAIN_COSMOS_