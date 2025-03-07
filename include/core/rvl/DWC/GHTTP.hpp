#pragma once
#include <kamek.hpp>

namespace DWC{
    namespace GHTTP {
        typedef void (*GHTTPCompletedCallback) (const char* buffer, u32 size, s32 ret, void* param);
        typedef void (*GHTTPProgressCallback) (s32 state, const void* buffer, u32 size, s32 byteRec, s32 totalSize, void* param);

        struct GHIPost {
            void* data;
            GHTTPCompletedCallback callback;
            void* param;
            u32 hasFiles;
            u32 hasSoap;
            u32 useDime;
            u32 autoFree;
        };

        typedef GHIPost* DWCGHTTPPost;

        void Init(const char*); //800cd3b0
        void Shutdown(); //800cd3f4
        void Process(); //800cd4a8

        s32 GetDataEx(const char* uri, u32 bufferSize, bool clearBuffer, GHTTPProgressCallback progressCB, GHTTPCompletedCallback completedCB, void* param); 
        void NewPost(DWCGHTTPPost* post);
        s32 PostData(const char* uri, DWCGHTTPPost* post, GHTTPCompletedCallback completeCB, void* param);
        s32 PostAddFileFromMemoryA(DWCGHTTPPost post, const char* name, const void* file, int size, const char* reportName, const char* contentType);
        
    }
}

enum GPIProgress {
    INIT,
    CREATE_SOCKET,
    HOST_LOOKUP,
    LOOKUP_PENDING,
    CONNECTION,
    SECURE_SESSION,
    SEND_REQUEST,
    POSTING,
    WAITING,
    RECEIVING_STATUS,
    RECEIVING_HEADERS,
    RECEIVING_FILE,
};

// Size unknown
struct _gpiConnection {
    u8 unknown_00[0x10-0x0]; 
    GPIProgress progress;
    char* url; //0x14
    char* host; //0x18
    u8 unknown_18[0x20-0x1C];
    u16 port;
    u16 padding;
    char* data;
    u32 isHttps;
};

extern "C" {
    bool gpiParseUrl(_gpiConnection* conn); // 
};
