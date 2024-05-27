#include <System/API/Updater.hpp>

namespace Cosmos
{
    void UpdateInfoCallback(u32 error, NHTTPResponse response, void* arg){
        ((Updater*)arg)->UpdateResponseCallback(error, response);
    }

    void* HTTPAlloc(u32 size, s32 align){
        void* ret = Updater::GetStaticInstance()->GetHTTPHeap()->alloc(size, align);
        return ret;
    }

    void HTTPFree(void* ptr){
        Updater::GetStaticInstance()->GetHTTPHeap()->free(ptr);
    }

    Updater* Updater::sInstance = nullptr;

    void Updater::CreateStaticInstance() {
        if(sInstance == nullptr) sInstance = new (RKSystem::mInstance.EGGSystem) Updater;
    }

    static BootHook bhCreateUpdaterInstance(Updater::CreateStaticInstance, LOW);

    void Updater::CheckForUpdate(){
        if(currentStage != IDLE) return;

        responseBuffer = httpHeap->alloc(0x100, 0x20);
        if(NHTTPStartup(HTTPAlloc, HTTPFree, 17) != 0){
            CosmosError("Failed to startup NHTTP library!\n");
            currentStage = UNABLE;
            return;
        }

        NHTTPRequest request = NHTTPCreateRequest(api_versionInfo, NHTTP_REQUEST_GET, responseBuffer, 0x100, UpdateInfoCallback, this);
        if(request == nullptr){
            CosmosError("Failed to create request!\n");
            currentStage = UNABLE;
            return;
        }
        NHTTPSetRootCADefault();
        NHTTPSetClientCertDefault(request);
        NHTTPSetVerifyOption(request, 0);
        NHTTPAddHeaderField(request, "User-Agent", "RVL SDK/1.0");

        s32 ret = NHTTPSendRequestAsync(request);
        if(ret < 0){
            CosmosError("Failed to send a request!\n");
            currentStage = UNABLE;
            return;
        }
        CosmosLog("Checking update data...\n");
        currentStage = CHECKING_VERSION;
    }

    void Updater::UpdateResponseCallback(u32 error, NHTTPResponse response){
        if(error != 0x0) {
            CosmosError("There was an error getting the request!\n");
            currentStage = UNABLE; return;
        }
        char* buffer;
        u32 retCode = NHTTPGetResultCode(response);
        if(retCode != 200){
            CosmosLog("Website returned: %d\n", retCode);
            currentStage = UNABLE; return;
        }

        s32 len = NHTTPGetBodyAll(response, &buffer);
        if(len != sizeof(UpdateInfo)){
            currentStage = UNABLE; return;
        }
        UpdateInfo* info = (UpdateInfo*)buffer;

        if(info->revision <= currentRevision){
            CosmosLog("You're on the newest version!\n");
            currentStage = FINISHED;
            NHTTPDestroyResponse(response);
        }

    }

    void Updater::CreateHeaps(){
        EGG::Heap* volatileMem2 = GameScene::GetCurrent()->structsHeaps.heaps[1];
        if(httpHeap == nullptr){
            void* block = volatileMem2->alloc(HTTP_HEAP_SIZE, 0x20);
            httpHeap = EGG::ExpHeap::Create(block, HTTP_HEAP_SIZE, 0);
            if(httpHeap == nullptr){
                CosmosError("Failed to create ExpHeap!");
                currentStage = UNABLE;
                return;
            }
        }
        if(fileDownloadHeap == nullptr){
            void* block = volatileMem2->alloc(FILE_HEAP_SIZE, 0x20);
            httpHeap = EGG::ExpHeap::Create(block, FILE_HEAP_SIZE, 0);
            if(httpHeap == nullptr){
                CosmosError("Failed to create ExpHeap!");
                currentStage = UNABLE;
                return;
            }
        }
    }

    void Updater::DestroyHeaps() {
        if(httpHeap != nullptr) httpHeap->destroy();
        if(fileDownloadHeap != nullptr) httpHeap->destroy();
    }

} // namespace Cosmos
