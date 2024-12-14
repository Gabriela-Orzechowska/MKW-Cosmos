#ifndef _MAIN_COSMOS_
#define _MAIN_COSMOS_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>
#include <System/LZMA.hpp>
#include <core/egg/DvdRipper.hpp>
#include <game/System/Archive.hpp>

#define AURORA

//#define NO_FAST_MENU

namespace Cosmos
{
    typedef void (*Console_Print_t)(const char* msg);
    typedef void (*Console_void_t)(void);
    

    enum CUSTOM_PAGE_IDS 
    {
        WARNING_PAGE = 0xBA,
        SETTINGS_MAIN = 0xB7,
        VARIANT_SELECT = 0xB8,
        SPINNER_WITH_BACKGROUND = 0xB9,
    };

    const char packFolder[] = "/Aurora";
    const char ghostFolder[] = "/Aurora/RKG";
    
    const char SaveFile[] = "AuroraData.bin";
    const char CommonArchive[] = "/CommonCosmos.szs";
    const char UIArchive[] = "Scene/UI/UICosmos.lzma";
    const char CourseArchive[] = "/CourseCosmos.szs";

    enum TT_MODE{
        COSMOS_TT_150cc,
        COSMOS_TT_200cc,
    };  

    class System{
    public:
        static void CreateStaticInstance();
        static inline System * GetStaticInstance() { return sInstance; }
        static Console_Print_t Console_PrintFunc;
        static Console_void_t Console_Clear;
    public:
        void Init();
        void CreateFolders();
        TT_MODE GetTTMode() { return this->currentTTMode; }
        void SetTTMode(TT_MODE mode);

        inline bool AreMiiHeadsAllowed() { return miiHeadsEnabled; }
        inline void SetMiiHeadSettings(bool setting) { miiHeadsEnabled = setting; }
        inline bool isHAW() { return haw; }
        inline void SetHAW(bool setting) { this->haw = setting; }
        inline void SetRaceCount(u8 raceCount) { this->raceCount = raceCount; }

        u8* GetTrackHash() { return currentTrackHash; }
        void SetTrackHash(u8* input) { memcpy(currentTrackHash, input, 20); }

        static void Shutdown();
        static void Shutdown(bool force);
        static void Restart();
        static void HardRestart();
        static inline void LoadLoaderFuncs() {
            Console_PrintFunc = (Console_Print_t) *((u32*)0x80003FEC);
            Console_Clear = (Console_void_t) *((u32*)0x80003FE4);
        }
        static inline void Console_Print(const char* msg){
            if(Console_PrintFunc != nullptr) Console_PrintFunc(msg);
        }

        static asm int PatchRaceCount();

    private:
        u8 currentTrackHash[20];
        static System * sInstance;
        TT_MODE currentTTMode;
        bool haw;
        bool miiHeadsEnabled;
        u8 raceCount;
    };

    IOS::IPCResult Open(char *path, IOS::Mode mode);
    void SetTTCC(TT_MODE mode);
    void SetCC();

    void LoadLZMAFile(ArchiveFile * file, const char * path, EGG::Heap *heap, bool isCompressed, s32 align, EGG::Heap * fileHeap, EGG::Archive::FileInfo * fileInfo);
    void CreateBranch(u32 from, void * to);
    void CreateCall(u32 from, void * to);
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK);
    void Panic(char * file, int line, char *fmt, ...);
}



#endif //_MAIN_COSMOS_
