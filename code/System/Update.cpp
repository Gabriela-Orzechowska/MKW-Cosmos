#include <kamek.hpp>
#include <core/rvl/nhttp/nhhtp.h>
#include <core/egg/Heap.hpp>
#include <core/System/RKSystem.hpp>

extern "C" void DWC_ProcessFriendsMatch();

static bool hasCheckedUpdate = false;
static bool isCheckingUpdate = false;
static EGG::Heap* httpHeap = nullptr;

void* HTTPAlloc(u32 size, s32 align){
    if(httpHeap == nullptr)
        httpHeap = RKSystem::mInstance.EGGSystem;
    httpHeap->alloc(size, align);
}

void HTTPFree(void* ptr){
    httpHeap->free(ptr);
}
void UpdateCallback(u32 error, NHTTPResponse response, void* args){
    CosmosLog("Response return error code: %d\nRequest Error: %d\n", error, NHTTPGetResultCode(response));
    hasCheckedUpdate = true;
    NHTTPDestroyResponse(response);
}

void ProcessUpdate(){
    
    if(hasCheckedUpdate){
        DWC_ProcessFriendsMatch();
        return;
    }
    if(isCheckingUpdate) return;
    char buffer[0x1000];
    if(NHTTPStartup(HTTPAlloc, HTTPFree, 17) != 0){
        CosmosError("Failed to init NHTTP Lib!\n");
        hasCheckedUpdate = true; return;
    }
    isCheckingUpdate = true;
    
    NHTTPRequest request = NHTTPCreateRequest("https://api.github.com/repos/Gabriela-Orzechowska/RANWr/releases/latest", NHTTP_REQUEST_GET, buffer, 0x1000, UpdateCallback, nullptr);
    if(request == nullptr){
        CosmosError("Failed to create request!\n");
        hasCheckedUpdate = true; return;
    }
    
    NHTTPSetRootCADefault();
    NHTTPSetClientCertDefault(request);
    NHTTPSetProxyDefault(request);
    NHTTPSetVerifyOption(request, 1);

    NHTTPAddHeaderField(request, "User-Agent", "CosmosUpdater");
    NHTTPAddHeaderField(request, "Host", "api.github.com");
    //NHTTPAddHeaderField(request, "Accept", "");
    //NHTTPAddHeaderField(request, "Accept-Encoding", "gzip, deflate, br");
    //NHTTPAddHeaderField(request, "Connection", "keep-alive");
    
    int requestId = NHTTPSendRequestAsync(request);
    if(requestId < 0){
        CosmosError("Failed to send a request!\n");
        hasCheckedUpdate = true; return;
    }
    CosmosLog("RequestId: %d\n", requestId);


}
kmCall(0x80657688, ProcessUpdate);
