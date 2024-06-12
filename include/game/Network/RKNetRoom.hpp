#ifndef _RKNETROOM_
#define _RKNETROOM_
#include <kamek.hpp>

enum ROOMRole{
    ROOM_HOST,
    ROOM_GUEST
};

enum ROOMMessageType{
    ROOM_START = 0x1,
    ROOM_REGISTER_FRIEND = 0x2,
    ROOM_JOIN_MESSAGE = 0x3,
    ROOM_CHAT_MESSAGE = 0x4
};

#pragma pack(push, 1)
struct ROOMPacket{
    u8 messageType; //see enum
    u16 message;
    u8 unknown_0x3;
}; //0x4
#pragma pack(pop)
static_assert(sizeof(ROOMPacket) == 0x4, "ROOMPacket");

union ROOMPacketReg{
    u32 packetArg;
    ROOMPacket packet;  
};


class RKNetROOMHandler{
public:
    static RKNetROOMHandler *sInstance; //809c20e0
    inline static RKNetROOMHandler *GetStaticInstance() { return sInstance; } //8065aa88
    static void DestroyStaticInstance(); //8065ab4c
    u32 GetRecvPacket(u8 id);
    void SetToSendPacket(u8 id, ROOMPacket packet);
    void SetAllToSendPackets(ROOMPacket packet);
    u8 unknown_0x0[4];
    ROOMRole role;
    ROOMPacket toSendPackets[12];
    ROOMPacket receivedPackets[12];
    u8 lastSentToAid;
    u8 unknown_0x69[0x70-0x69];
    u64 lastSentTime;
    u32 unknown_0x78[2];
}; //total size 0x80
static_assert(sizeof(RKNetROOMHandler) == 0x80, "RKNetROOMHandler");
#endif
