#pragma once
#include <kamek.hpp>
#include <game/Network/RKNetSelect.hpp>
#include <game/Network/RKNetController.hpp>


namespace CosmosNetwork
{
    struct CosmosSELECTPacket { // Based on lecode one
        u64 timeSender;
        u64 timeReceived;
        SELECTPlayerData playersData[2]; //0x10
        u32 selectId; //0x20
        u8 battleType;
        u32 teams : 24; //0x25
        u8 playerIdToAid[12]; //0x28
        u16 winningCourse; //0x34
        u8 winningVoterAid; //0x36
        u8 engineClassAndPhase; //0x37 none, 100, 150, mirror

        u8 GetPhase() { return ((engineClassAndPhase & 0xF0) >> 4); }
        u8 GetEngineClass() {return (engineClassAndPhase & 0x0F); }
    };
    static_assert(sizeof(CosmosSELECTPacket) == 0x38, "CosmosSELECTPacket");

    class RKNetSELECTHandlerPlus {
        public:
        static RKNetSELECTHandlerPlus *sInstance; //0x809c2100
        static RKNetSELECTHandlerPlus * GetStaticInstance() {return sInstance;}
        
        OnlineMode mode; //from page 0x90 OnInit MenuId Switch
        u32 unknown_0x4;
        CosmosSELECTPacket toSendPacket; //0x8
        CosmosSELECTPacket receivedPackets[12]; //0x40
        u8 lastSentToAid; //0x2e0
        u8 unknown_0x2e4[7];
        u64 lastSentTime; //0x2e8
        u64 lastReceivedTimes[12]; //0x2f0
        u64 unknown_0x350[12]; //0x350
        u32 unknown_0x3b0[12]; //0x3b0
        u32 hasNewSELECT; //0x3e0 bitflag
        u32 hasNewRACEHEADER_1; //0x3e4 bitflag
        u32 unknown_aidBitflag; //0x3e8
        u32 unknown_aidBitFlag2; //0x3ec
        u32 hasVoted; //0x3f0
        u8 unknown_0x3F4[4];
    };

} // namespace Cosmos

