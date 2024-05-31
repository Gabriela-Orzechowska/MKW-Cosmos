#pragma once
#include <kamek.hpp>
#include <core/rvl/nhttp/nhhtp.h>
#include <core/egg/Heap.hpp>
#include <core/System/RKSystem.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Scene/BaseScene.hpp>

#define HTTP_HEAP_SIZE 0x1A000
#define MAX_RESPONSE_SIZE 0x3000

namespace Cosmos
{
    const u32 currentRevision = 2;
    const char api_link[] = "http://cosmos.gabriela-orzechowska.com/andromeda/api/%s";
    const char* api_endpoints[] = {
        "get-version-info/current",
    };

    class APIUser;

    class API{
    public:
        enum AndroAPICall {
            GET_CURRENT_VERSION_INFO = 0x0,
        };

        enum Stage {
            IDLE,
            BUSY,
            UNABLE,
        };

        enum RET{
            SUCCESS = 0x0,
            UNAVAILABLE = -0x1,
            INTERNAL_ERROR = -0x2,
            INVALID_USER = -0x3,
            INVALID_BUFFER = -0x4,
        };

        API() : stage(IDLE), isReady(false), user(nullptr), requestHeap(nullptr) {};
        ~API() { DestroyHeap(); }

        inline static API* GetStaticInstance() { return sInstance; }
        static void CreateStaticInstance();

        void ResponseCallback(u32 error, NHTTPResponse response);
        inline EGG::Heap* GetHeap() { return requestHeap; }

        RET Request(AndroAPICall type, void* responseBuffer, u32 bufferSize, APIUser* user); 

        void CreateHeap();
        void DestroyHeap();

    private:
        static API* sInstance;
        bool isReady;
        Stage stage;
        EGG::Heap* requestHeap;
        void* currentBuffer;

        APIUser* user;
        void* userBuffer;
        u32 userBufferSize;
    };

    class APIUser{
    public:
        virtual void ResponseCallback(u32 ret);
    };
    
} // namespace Cosmos
