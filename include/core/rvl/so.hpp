#pragma once
#include <kamek.hpp>

extern "C" {

typedef void* (*SOAlloc)(u32 r3, s32 size);
typedef void (*SOFree)(u32 r3, void* ptr);

#define SO_PF_INET 2
#define SO_SOCK_STREAM 1
#define SO_SOCK_DGRAM 2

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
    s32 flag; // 0x00
    s32 family; // 0x04
    s32 socketType; //0x08
    s32 protocol; //0x0C
    u32 addrLen; //0x10
    char* name; //0x14
    void* addr; //0x18
    SOAddrInfo* next; //0x1C
};

struct SOLibraryConfig {
    SOAlloc alloc;
    SOFree free;
};

s32 SOInit(SOLibraryConfig* cfg);
s32 SOStartup();
s32 SOCleanup();
s32 SOFinish(); //801ec088
s32 SOSocket(s32 domain, s32 type, s32 protocol);
s32 SOConnect(s32 handle, SOSockAddrIn* addr);
s32 SOClose(s32 socket);
s32 SOSend(s32 handle, const void* buf, u32 size, u32 flags);
s32 SORecv(s32 handle, const void* buf, u32 size, u32 flags);
s32 SOGetAddrInfo(const char* name, const char* serviceName, const SOAddrInfo* info, SOAddrInfo** ret);
s32 SOSetSockOpt(s32 socket, s32 level, s32 optName, const void* val, s32 len);
void SOFreeAddrInfo(SOAddrInfo* info);

}
