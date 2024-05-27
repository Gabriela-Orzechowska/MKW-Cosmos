#pragma once
#include "types.hpp"

typedef enum {
    NHTTP_REQUEST_GET,
    NHTTP_REQUEST_POST,
} NHTTP_REQUEST;

typedef void* NHTTPResponse;
typedef void* NHTTPRequest;

typedef void (*NHTTPRequestCallback)(u32 error, NHTTPResponse responce, void* args);
typedef void* (*NHTTPAlloc)(u32 size, s32 align);
typedef void (*NHTTPFree)(void* ptr);

extern "C"
{
    s32 NHTTPStartup(NHTTPAlloc, NHTTPFree, u32 prio); //801d8d30
    NHTTPResponse NHTTPCreateRequest(const char* url, NHTTP_REQUEST requestMethod, void* responseBuffer, u32 responseSize, NHTTPRequestCallback callbackMethod, void* args); //801d8ff8
    s32 NHTTPSetRootCADefault(); //801d9738 
    s32 NHTTPSetClientCertDefault(NHTTPRequest request); //801d9798
    s32 NHTTPSetProxyDefault(NHTTPRequest request); //801d9610
    s32 NHTTPSetVerifyOption(NHTTPRequest request, u32 type); //801d9444
    s32 NHTTPAddHeaderField(NHTTPRequest request, const char* fieldName, const char* fieldValue);
    s32 NHTTPSendRequestAsync(NHTTPRequest requires);
    s32 NHTTPCancelRequestAsync(int requestId);
    void NHTTPDestroyResponse(NHTTPResponse response);
    s32 NHTTPGetBodyAll(NHTTPResponse response, char** reponseBody);
    s32 NHTTPGetResultCode(NHTTPResponse response);
}

