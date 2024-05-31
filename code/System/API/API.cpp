#include <System/API/API.hpp>

namespace Cosmos
{
    void _ResponseCallback(u32 error, NHTTPResponse response, void* arg){
        ((API*)arg)->ResponseCallback(error, response);
    }

    void* HTTPAlloc(u32 size, s32 align){
        void* ret = API::GetStaticInstance()->GetHeap()->alloc(size, align);
        return ret;
    }

    void HTTPFree(void* ptr){
        API::GetStaticInstance()->GetHeap()->free(ptr);
    }
    
    API* API::sInstance = nullptr;

    void API::CreateStaticInstance(){
        if(sInstance == nullptr) sInstance = new (RKSystem::mInstance.EGGSystem) API;
    }
    static BootHook bhCreateAPIInstance(API::CreateStaticInstance, LOW);

    void API::CreateHeap(){
        EGG::Heap* volatileMem2 = GameScene::GetCurrent()->structsHeaps.heaps[1];
        if(requestHeap == nullptr){
            void* block = volatileMem2->alloc(HTTP_HEAP_SIZE, 0x20);
            requestHeap = EGG::ExpHeap::Create(block, HTTP_HEAP_SIZE, 0);
            if(requestHeap == nullptr){
                CosmosError("Failed to create ExpHeap!");
                stage = UNABLE;
                return;
            }
        }
    }

    void API::DestroyHeap(){
        if(requestHeap != nullptr) requestHeap->destroy();
    }

    API::RET API::Request(AndroAPICall call, void* responseBuffer, u32 bufferSize, APIUser* user){
        if(!user) return INVALID_USER;
        if(!responseBuffer) return INVALID_BUFFER;

        if(stage != IDLE) return UNAVAILABLE;

        stage = BUSY;

        currentBuffer = requestHeap->alloc(MAX_RESPONSE_SIZE, 0x20);
        if(NHTTPStartup(HTTPAlloc, HTTPFree, 17) != 0){
            stage = UNABLE;
            return INTERNAL_ERROR;
        }
        char uri[0x60];
        snprintf(uri, 0x60, api_link, api_endpoints[(int)call]);
        NHTTP_REQUEST type = call < 0x100 ? NHTTP_REQUEST_GET : NHTTP_REQUEST_POST; 

        NHTTPRequest request = NHTTPCreateRequest(uri, type, currentBuffer, MAX_RESPONSE_SIZE, _ResponseCallback, this);

        if(request == nullptr){
            stage = UNABLE;
            return INTERNAL_ERROR;
        }

        NHTTPSetRootCADefault();
        NHTTPSetClientCertDefault(request);
        NHTTPSetVerifyOption(request, 0);
        NHTTPAddHeaderField(request, "User-Agent", "ANDRO API/1.0");

        if(NHTTPSendRequestAsync(request) < 0){
            stage = UNABLE;
            return INTERNAL_ERROR;
        }      

        this->user = user;
        this->userBuffer = responseBuffer;  
        this->userBufferSize = bufferSize;    
        return SUCCESS;
    }

    void API::ResponseCallback(u32 error, NHTTPResponse response){
        if(error != 0x0){
            user->ResponseCallback(error);
            return;
        }

        u32 retCode = NHTTPGetResultCode(response);
        char* responseBody;
        NHTTPGetBodyAll(response, &responseBody);
        memcpy(this->userBuffer, responseBody, userBufferSize);
        NHTTPDestroyResponse(response);
        user->ResponseCallback(retCode == 200 ? 0 : retCode);
    }
} // namespace Cosmos
