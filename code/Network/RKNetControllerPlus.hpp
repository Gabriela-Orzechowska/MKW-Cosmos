#pragma once
#include <kamek.hpp>
#include <game/Network/RKNetSelect.hpp>
#include <game/Network/RKNetController.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <game/System/Random.hpp>
#include <Settings/UserData.hpp>
#include <game/System/Archive.hpp>


namespace CosmosNetwork
{
    #pragma pack(push, 1)
    struct CosmosSELECTPlayerData{
        u16 cCourseVote;
        u16 sumPoints;
        u8 character;
        u8 kart;
        u8 cPrevRank;
        u8 starRank;
    }; //total size 0x8
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct CosmosSELECTPacket { // Based on lecode one
        u64 timeSender;
        u64 timeReceived;
        CosmosSELECTPlayerData playersData[2]; //0x10
        u32 selectId; //0x20
        u8 battleType;
        u32 teams : 24; //0x25
        u8 playerIdToAid[12]; //0x28
        u16 winningCourse; //0x34
        u8 winningVoterAid; //0x36
        u8 phase : 4;
        u8 engineClass : 4;
    };
    #pragma pack(pop)
    static_assert(sizeof(CosmosSELECTPacket) == 0x38, "CosmosSELECTPacket");

    #pragma pack(push, 1)
    struct CosmosRH1Packet {
        u32 timer;
        u32 selectId; //0x4
        u16 team[2]; //0x8
        u16 lagFrames; //0xc
        u16 kartAndCharacter[2]; //0xe treated as a u16 ingame
        u16 countdownTime; //0x12
        u8 starCount1 : 4;
        u8 starCount2 : 4;
        u16 trackId;
        u8 unknown_0x17;
        u8 aidsBelongToPlayer[12]; //0x18
        u8 engineClass;
        u8 unknown_0x25[3];
    };
    #pragma pack(pop)
    static_assert(sizeof(CosmosRH1Packet) == 0x28, "CosmosRH1Packet");

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

