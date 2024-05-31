#pragma once
#include <System/API/API.hpp>

extern "C" void DWC_ProcessFriendsMatch();

#define HTTP_HEAP_SIZE 0x1A000
#define FILE_HEAP_SIZE 0x70000

namespace Cosmos
{
    const u32 currentRevision = 2;

    const char api_link[] = "http://cosmos.gabriela-orzechowska.com/andromeda/api/%s";
    const char api_versionInfo[] = "get-version-info/current";
    
    class Updater: APIUser {

    enum Stage{
        IDLE = 0x0,
        CHECKING_VERSION,
        CHECKING_FILE,
        DOWNLOADING,
        FINISHED,
        UNABLE,
    };

    struct UpdateInfo {
        u32 revision;
        u32 fileSize;
    };

    public:

        Updater() : currentStage(IDLE), areHeapsReady(false), httpHeap(nullptr), fileDownloadHeap(nullptr) {}
        ~Updater() { DestroyHeaps(); }

        static Updater* GetStaticInstance() { return sInstance; }
        static void CreateStaticInstance();

        void CheckForUpdate();
        void UpdateResponseCallback(u32 error, NHTTPResponse response);

        void CreateHeaps();
        void DestroyHeaps();

        EGG::Heap* GetHTTPHeap() { return httpHeap; }

    private:
        static Updater* sInstance;
        Stage currentStage;
        bool areHeapsReady;
        EGG::Heap* httpHeap;
        EGG::Heap* fileDownloadHeap;
        void* responseBuffer;
    };
} // namespace Cosmos
