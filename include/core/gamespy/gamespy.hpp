#pragma once

#include <kamek.hpp>

extern "C" {
namespace GameSpy {
    typedef void* GPConnection;
    
    typedef enum {
        GPNoError = 0,
    } GPRet;

    typedef enum {
        GT2Success = 0,
        GT2OOM,
        GT2Rejected,
        GT2NetworkError,
        GT2AddressError,
        GT2DuplicateAddress,
        GT2Timeout,
        GT2NegotiationError,
        GT2InvalidConnection,
        GT2InvalidMessage,
        GT2SendFailed,
    } GT2Ret;

#define GP_AUTHTOKEN_LEN 256
#define GP_PARTNERCHALLENGE_LEN 256
#define GP_CDKEY_LEN 65

    typedef enum {
        GPIFalse,
        GPITrue,
    } GPIBool;

    typedef struct {
        char* buffer;
        int size;
        int length;
        int position;
    } GPIBuffer;

    typedef struct {
        char serverChallenge[128];
        char userChallenge[33];
        char passwordHash[33];
        char authtoken[GP_AUTHTOKEN_LEN];
        char partnerchallenge[GP_PARTNERCHALLENGE_LEN];
        char cdkey[GP_CDKEY_LEN];
        GPIBool newuser;
    } GPIConnectData;

    typedef struct {
        u8 unknown_00[0x210];
        GPIBuffer outputBuffer;
        u8 unknown_220[0x5E0-0x220];
        GPIBuffer updateproBuffer;
    } GPIConnection;

    GPRet gpiAppendStringToBuffer(GPConnection* connection, GPIBuffer* output, const char* buffer);
    int gpiAppendIntToBuffer(GPConnection* connection, GPIBuffer* output, int val);
    GPIBool gpiValueForKey(const char* command, const char* key, char* value, int size);
    GPRet gpiSendLocalInfo(GPConnection* connection, const char* key, const char* value) {
        GPIConnection* gpiConnection = (GPIConnection*)(*connection);
        GPRet ret = gpiAppendStringToBuffer(connection, &gpiConnection->updateproBuffer, key);
        if(ret != GPNoError) return ret;

        return gpiAppendStringToBuffer(connection, &gpiConnection->updateproBuffer, value);
    }

}

}
