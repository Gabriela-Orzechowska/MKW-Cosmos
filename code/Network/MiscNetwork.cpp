#include "Settings/UserData.hpp"
#include "core/rvl/rvl_sdk.hpp"
#include "main.hpp"
#include <kamek.hpp>
#include <core/System/SystemManager.hpp>
#include <game/Network/RKNetRoom.hpp>
#include <game/Network/RKNetController.hpp>
#include <Network/SHA256.hpp>
#include <Network/WiiLinkTypes.hpp>

#define TEST_REGION 2137 

void WriteAndInvalidate(u32* address, u32 val){
    *address = val;
    register u32 addr = (u32) address;
    asm{
        ASM(
            dcbst 0, addr;
            sync;
            icbi 0, addr;
        );
    }
}

void ChangeGameRegion(u32 regionID)
{
    extern u32 pr_patch1_r5;
    extern u32 pr_patch2_r6;
    extern u32 pr_patch3_r7;
    extern u32 pr_patch4_r7;
    extern u32 pr_patch5_r7;
    extern u32 pr_patch6_r7;
    extern u32 pr_patch7_r4;
    extern u32 pr_patch8_r4;

    WriteAndInvalidate(&pr_patch1_r5, 0x38A00000 | regionID);
    WriteAndInvalidate(&pr_patch2_r6, 0x38C00000 | regionID);
    WriteAndInvalidate(&pr_patch3_r7, 0x38E00000 | regionID);
    WriteAndInvalidate(&pr_patch4_r7, 0x38E00000 | regionID);
    WriteAndInvalidate(&pr_patch5_r7, 0x38E00000 | regionID);
    WriteAndInvalidate(&pr_patch6_r7, 0x38E00000 | regionID);
    WriteAndInvalidate(&pr_patch7_r4, 0x38800000 | regionID);
    WriteAndInvalidate(&pr_patch8_r4, 0x38800000 | regionID);
    WWFC_CUSTOM_REGION = regionID;
}

void ChangeRegionAtBoot(){
    ChangeGameRegion(TEST_REGION);
}

// TODO why is it here
// kmWrite16(0x8088fe30, 0x6478); //rksys -> dxsys

static BootHook bhChangeRegionAtBoot(ChangeRegionAtBoot, LOW);

const u8 raceCounts[8] = {2, 4, 8, 12, 16, 32, 64};

void ParseHostSettings(u16 value) {
    bool haw = value & 0x1;
    bool miiHeads = (value >> 1) & 0x1;
    u8 raceCount = raceCounts[(value >> 2) & 0x7];
    bool megaTc = (value >> 5) & 0x1;
    bool variantSelection = (value >> 6) & 0x1;
    bool randomForce = (value >> 7) & 0x1;
    bool randomCommon = (value >> 8) & 0x1;

    Cosmos::System* system = Cosmos::System::GetStaticInstance();
    system->SetHAW(haw);
    system->SetMiiHeadSettings(miiHeads);
    system->SetRaceCount(raceCount);

    Cosmos::Data::SettingsHolder* holder = Cosmos::Data::SettingsHolder::GetStaticInstance();
    holder->SetMegaCloudSetting(megaTc);
    holder->SetChooseVariant(variantSelection);
    holder->SetRandomComboOptions(randomForce, randomCommon);
}

void BeforeSendingPackets(RKNetROOMHandler& handler, u32 packetData) {
    using namespace Cosmos::Data;
    RKNetController* controller = RKNetController::GetStaticInstance();
    u8 userAid = controller->subs[controller->currentSub].localAid;
    u8 hostAid = controller->subs[controller->currentSub].hostAid;
    
    ROOMPacketReg packet = { packetData }; 

    if((packet.packet.messageType == 1 || packet.packet.messageType == 5) && userAid == hostAid) {
        packet.packet.unknown_0x3 = (u8) packet.packet.message;
        u16 settings = 0;
        SettingsHolder* holder = SettingsHolder::GetStaticInstance();
        settings |= ((holder->GetSettingValue(COSMOS_SETTING_HAW)) == ENABLED) << 0; 
        settings |= ((holder->GetSettingValue(COSMOS_SETTING_MII_HEADS)) == ENABLED) << 1; 
        settings |= ((holder->GetSettingValue(COSMOS_SETTING_RACE_COUNT) & 0x7)) << 2;
        settings |= ((holder->GetSettingValue(COSMOS_SETTING_HOST_MEGA_TC) == ENABLED)) << 5;
        settings |= ((holder->GetSettingValue(COSMOS_SETTING_HOST_VARIANT_SELECTION) == ENABLED)) << 6;

        bool randomForce = holder->GetSettingValue(AURORA_SETTING_HOST_RANDOM_COMBO) != RANDOM_COMBO_DISABLED;
        bool randomCombo = holder->GetSettingValue(AURORA_SETTING_HOST_RANDOM_COMBO) == RANDOM_COMBO_COMMON;

        settings |= randomForce << 7;
        if(randomForce)
            settings |= randomCombo << 8;

        ParseHostSettings(settings);

        packet.packet.message = settings;
    }
    for(int i = 0; i < 12; i++) {
        if(i != userAid) handler.toSendPackets[i] = packet.packet;
    }
}
kmBranch(0x8065ae70, BeforeSendingPackets);

ROOMPacket BeforeReadingPackets(RKNetROOMHandler& handler, u32 packetIndex) {
    ROOMPacket packet = handler.receivedPackets[packetIndex];

    if(packet.messageType == 1 || packet.messageType == 5) {
        u16 settings = packet.message;

        ParseHostSettings(settings);

        packet.message = packet.unknown_0x3;
        packet.unknown_0x3 = 0;

        if(packet.messageType == 5) packet.messageType = 0;
    }

    return packet;
}
kmBranch(0x8065af64, BeforeReadingPackets);
