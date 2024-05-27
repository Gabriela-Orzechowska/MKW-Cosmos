#include <kamek.hpp>
#include <core/rvl/nhttp/nhhtp.h>
#include <core/egg/Heap.hpp>
#include <core/System/RKSystem.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Scene/BaseScene.hpp>

extern "C" void DWC_ProcessFriendsMatch();

static bool hasCheckedUpdate = false;
static bool isCheckingUpdate = false;
static EGG::Heap* httpHeap = nullptr;
static NHTTPRequest grequest = nullptr;

void* HTTPAlloc(u32 size, s32 align){
    void* ret = httpHeap->alloc(size, align);
    return ret;
}

#define HTTP_HEAP_SIZE 0x1A000

static char* responceBuffer = nullptr;

void HTTPFree(void* ptr){
    httpHeap->free(ptr);
}

void UpdateCallback(u32 error, NHTTPResponse response, void* args){
    if(error == 0xE){
        CosmosError("SSL Error while sending a request!\n");
    }
    hasCheckedUpdate = true;
    
    char* responceBuffer;
    s32 len = NHTTPGetBodyAll(response, &responceBuffer);
    CosmosLog("Request error: %03x, responce error: %d, Body: %p, len: %d\n", error, NHTTPGetResultCode(response), responceBuffer, len);
    NHTTPDestroyResponse(response);
}

void CreateHTTPHeap(){
    if(httpHeap == nullptr) {
        void* allocBlock = GameScene::GetCurrent()->structsHeaps.heaps[1]->alloc(HTTP_HEAP_SIZE, 0x20);
        //void* allocBlock = new (RKSystem::mInstance.EGGRootMEM2, 0x20) char[0xA860];
        if(allocBlock != nullptr) {
            httpHeap = EGG::ExpHeap::Create(allocBlock, HTTP_HEAP_SIZE, 0);
            if(httpHeap == nullptr) {
                CosmosError("Failed to Create ExpHeap!\n");
                hasCheckedUpdate = true; return;
            }
            else{
                CosmosLog("Set the ExpHeap successfully\n");
            }
        }
        else {
            CosmosError("Failed to allocate block!\n");
            hasCheckedUpdate = true; return;
        }
    }
}
kmBranch(0x8064bc8c, CreateHTTPHeap);

void ProcessUpdate(){
    if(hasCheckedUpdate){
        DWC_ProcessFriendsMatch();
        return;
    }
    if(isCheckingUpdate) return;
    responceBuffer = (char*) httpHeap->alloc(0x1000, 0x20);
    if(NHTTPStartup(HTTPAlloc, HTTPFree, 17) != 0){
        CosmosError("Failed to init NHTTP Lib!\n");
        hasCheckedUpdate = true; return;
    }
    isCheckingUpdate = true;
    
    NHTTPRequest request = NHTTPCreateRequest("http://cosmos.gabriela-orzechowska.com/andromeda/api/get-version-info/current", NHTTP_REQUEST_GET, responceBuffer, 0x1000, UpdateCallback, nullptr);
    if(request == nullptr){
        CosmosError("Failed to create request!\n");
        hasCheckedUpdate = true; return;
    }
    grequest = request;
    s32 ret = NHTTPSetRootCADefault();
    if(ret != 0){
        CosmosError("Failed to set CA!\n");
        hasCheckedUpdate = true; return;
    }
    ret = NHTTPSetClientCertDefault(request);
    if(ret != 0){
        CosmosError("Failed to set Certificate!\n");
        hasCheckedUpdate = true; return;
    }
    //NHTTPSetProxyDefault(request);
    NHTTPSetVerifyOption(request, 0);

    NHTTPAddHeaderField(request, "User-Agent", "RVL SDK/1.0");
    //NHTTPAddHeaderField(request, "Host", "raw.githubusercontent.com");
    NHTTPAddHeaderField(request, "Accept", "");
    NHTTPAddHeaderField(request, "Accept-Encoding", "gzip, deflate, br");
    NHTTPAddHeaderField(request, "Connection", "keep-alive");
    
    int requestId = NHTTPSendRequestAsync(request);
    if(requestId < 0){
        CosmosError("Failed to send a request!\n");
        hasCheckedUpdate = true; return;
    }
    CosmosLog("Sending request to: http://cosmos.gabriela-orzechowska.com/andromeda/api/get-version-info/current\nRequestId: %d\n", requestId);


}
//static BootHook AttemptUpdateCheck(ProcessUpdate, LOW);
kmCall(0x80657688, ProcessUpdate);
