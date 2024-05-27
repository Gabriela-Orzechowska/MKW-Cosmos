#pragma once
#include <kamek.hpp>

typedef void* (*SOAlloc)(u32 size, s32 align);
typedef void (*SOFree)(void* ptr);

struct SOInAddr {
    u32 addr;    
};

struct SOSockAddrIn {
    u8 len;
    u8 family;
    u16 port;
    SOInAddr addr;
};

struct SOAddrInfo {
    s32 flag;
    s32 family;
    s32 socketType;
    s32 protocol;
    u32 addrLen;
    char* name;
    void* addr;
    SOAddrInfo* next;
};

s32 SOSocket(s32 domain, s32 type, s32 protocol);
s32 SOClose(s32 socket);
s32 SOGetAddrInfo(const char* name, const char* serviceName, const SOAddrInfo* info, SOAddrInfo** ret);
s32 SOSetSockOpt(s32 socket, s32 level, s32 optName, const void* val, s32 len);
void SOFreeAddrInfo(SOAddrInfo* info);

