#pragma once
#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <game/Visual/Mii.hpp>

#pragma pack(push, 1)
struct USERPacket{
    u32 recordId; // 00
    u16 miiCount; //04
    u16 unknown_0x06; //06
    u8 mii[0xA0-0x08];
    u64 wii_fc; //a0
    u64 friend_code; //a8
    u8 country; //b0
    u8 state; //b1
    u16 city; //b2
    u16 longitude; //b4
    u16 latitude; //b6
    u16 vr; //b8
    u16 br; //ba
    u8 game_id; //bc
    u8 region; //bd
    u16 unknown_0xbe; //be
}; //0xc0 
#pragma pack(pop)
size_assert(USERPacket, 0xc0);
// setting vr 805d302c
class RKNetUSERHandler
{
    public:
    static RKNetUSERHandler * sInstance; // 809c2108
    static RKNetUSERHandler * GetStaticInstance() { return sInstance; }
    static void CreateStaticInstance(); // 80662778
    static void DestroyStaticInstance(); // 806627f8

    void SendLocalVR(); //80662d80
    void SendLocalBR(); //80662dc4
    void BuildUserPacket(); //8066300c
    void Prepare(); //8066284c ?
    void Reset(); //80662944
    void Update(); //806629c0
    void CopySendPacketToController(u32 aid); // 80662abc
    void OnPlayerDisconnect(u32 aid); //80662b00
    void UpdateReceivedPackets(); //80662ebc

    u8 unknown_bool; //00
    u8 unknown_0x2[7]; //01
    USERPacket toSendPacket; //08
    USERPacket receivedPackets[12]; //c8
    u8 unknown[0x9f0-0x9c8]; //9c8
};
size_assert(RKNetUSERHandler, 0x9f0);
//sInstance  //at 0x180 we have enemy VR