/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Network/RKNetController.hpp"
#include "Settings/UserData.hpp"
#include "SlotExpansion/CupManager.hpp"
#include "core/rvl/os/OS.hpp"
#include <Network/RKNetControllerPlus.hpp>
#include <game/Network/RKNetPlayerInfo.hpp>
#include <game/UI/Page/Other/Votes.hpp>

namespace CosmosNetwork
{
    /*
    void PatchSELECTTrack(PacketHolder& holder, CosmosSELECTPacket* packet, u32 len){

    }
*/

    //Ignore player 2 Votes
    kmWrite32(0x80660594, 0x38000000);
    kmWrite32(0x806605ac, 0x38000000);


    u8 GetActualEngineClass(RKNetSELECTHandlerPlus& handler) {
        if(handler.toSendPacket.phase != 0) return handler.toSendPacket.engineClass;
        return 0;
    }
    kmBranch(0x8066048c, GetActualEngineClass);

    u16 GetActualWinningCourse(RKNetSELECTHandlerPlus& handler) {
        if(handler.toSendPacket.phase == 2) return handler.toSendPacket.winningCourse;
        return 0xFF;
    }
    kmBranch(0x80660450, GetActualWinningCourse);

    bool HasDecided(RKNetSELECTHandlerPlus& handler) {
        return handler.toSendPacket.winningCourse != 0xFF;
    }
    kmBranch(0x80660d40, HasDecided);

    u32 CorrectRandomSelection() {
        return Cosmos::CupManager::GetStaticInstance()->GetRandomTrack();
    }
    kmCall(0x80661f34, CorrectRandomSelection);

    void DecideTrack(RKNetSELECTHandlerPlus& handler) {
        Random random;
        Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
        RKNetController* rkController = RKNetController::GetStaticInstance();
        RKNetControllerSub& rkControllerSub = rkController->subs[rkController->currentSub];
        
        bool hostAlwaysWin = Cosmos::System::GetStaticInstance()->isHAW(); //Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_HOST_ALWAYS_WINS) == Cosmos::Data::ENABLED;

        if(hostAlwaysWin && handler.mode == ONLINEMODE_PRIVATE_VS){
            u16 hostTrack = handler.toSendPacket.playersData[0].cCourseVote;
            if(hostTrack == 0xFF) hostTrack == manager->GetRandomTrack();
            handler.toSendPacket.winningCourse = hostTrack;
            handler.toSendPacket.winningVoterAid = rkControllerSub.hostAid;
        }
        else if(handler.mode == ONLINEMODE_PUBLIC_VS || handler.mode == ONLINEMODE_PRIVATE_VS){
            u32 availableAids = rkControllerSub.availableAids;

            u8 playerCount = 0;
            u8 nonRepeatPlayerCount = 0;
            u8 playerAids[12];
            u8 nonRepeatPlayerAids[12];

            int index = 0;
            for(int i = 0; i < 12; i++){
                if(((1 << i) & availableAids) == 0x0) continue; //aid not found
                
                playerAids[playerCount++] = i;
                u32 playerVote = i == rkControllerSub.localAid ? handler.toSendPacket.playersData[0].cCourseVote : handler.receivedPackets[i].playersData[0].cCourseVote;

                bool repeatVote = false;
                for(int i = 0; i < TRACK_BLOCK_COUNT; i++){
                    u32 track = manager->GetTrackBlockByIndex(i);
                    if(track == ~0x0) continue;
                    if(track == playerVote) {
                        repeatVote = true; break;
                    }
                }
                if(!repeatVote)
                    nonRepeatPlayerAids[nonRepeatPlayerCount++] = i;
            }
            u8 winnerAid = 0;
            if (nonRepeatPlayerCount > 0) winnerAid = nonRepeatPlayerAids[random.NextLimited(nonRepeatPlayerCount)];
            else winnerAid = playerAids[random.NextLimited(playerCount)];

            u16 actualVote = winnerAid == rkControllerSub.localAid ? handler.toSendPacket.playersData[0].cCourseVote : handler.receivedPackets[winnerAid].playersData[0].cCourseVote;
            if(actualVote == 0xFF) actualVote = manager->GetRandomTrack();
            manager->AddTrackToBlocking(actualVote);
            if(Cosmos::isGroupSlot(actualVote)) actualVote = manager->GetRandomVariantTrack(actualVote);
            handler.toSendPacket.winningCourse = actualVote;
            handler.toSendPacket.winningVoterAid = winnerAid;
            CosmosLog("Winner track: %03x\n", actualVote);
        }
        else ((RKNetSELECTHandler*)&handler)->DecideTrack();
    }
    kmCall(0x80661490, DecideTrack);

#define CC_100_PROBS 15
#define CC_MIRROR_PROBS 15
#define CC_150_PROBS 60

    void DecideCC(RKNetSELECTHandlerPlus& handler) {
        RKNetController* controller = RKNetController::GetStaticInstance();
        RKNetSearchType type = controller->searchType;
        
        u8 ccEngineValue = 2;
        Cosmos::Data::FORCE_CC ccSetting = (Cosmos::Data::FORCE_CC) Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_FORCE_CC);
        if(type == VS_WW || type == VS_REGIONAL || (type == FROOM_HOST && ccSetting == Cosmos::Data::FORCE_NONE)) {
            Random random(OSGetTick());
            u32 ret = random.NextLimited(100);
            if(ret < CC_100_PROBS) ccEngineValue = 1;
            else if(ret < (CC_100_PROBS + CC_MIRROR_PROBS)) ccEngineValue = 3;
        }
        else if (type == FROOM_HOST) {
            if(ccSetting == Cosmos::Data::FORCE_150CC) ccEngineValue = 2;
            if(ccSetting == Cosmos::Data::FORCE_200CC) ccEngineValue = 1;
            if(ccSetting == Cosmos::Data::FORCE_MIRROR) ccEngineValue = 3;
        }
        handler.toSendPacket.engineClass = ccEngineValue; 
    }
    kmCall(0x80661404, DecideCC);

    u32 SetCorrectSlot(RKNetSELECTHandlerPlus& handler) {
        if(handler.toSendPacket.phase != 0) return Cosmos::CupManager::GetStaticInstance()->GetCurrentTrackSlot();
        return GetActualWinningCourse(handler);
    }
    kmCall(0x80650ea8, SetCorrectSlot);

    void LoadCorrectTrack(ArchiveRoot& root, u32 winningCourse){
        Cosmos::CupManager::GetStaticInstance()->SetWinningTrack(winningCourse);
        //root.RequestLoadCourseAsync((CourseId)winningCourse);
    }
    kmCall(0x80644414, LoadCorrectTrack);

    void PatchRaceHeader1(PacketHolder& holder, RACEHEADER1Packet* packet, u32 len){
        CosmosRH1Packet* cpacket = (CosmosRH1Packet*) packet;
        cpacket->starCount1 = packet->starCount[0];
        cpacket->starCount2 = packet->starCount[1];
        cpacket->trackId = Cosmos::CupManager::GetStaticInstance()->GetWinningTrack();
        holder.Copy(cpacket, len);
    }

    kmCall(0x80655458, PatchRaceHeader1);
    kmCall(0x806550e4, PatchRaceHeader1);
    
    void ImportPatchRaceHeader1() {
        register CosmosRH1Packet* packet;
        register RH1Data* data;
        asm{
            ASM(
                mr packet, r28;
                mr data, r26;
                addi data, data, 0x20;
            )
        }
        data->trackId = (CourseId) packet->trackId;
        data->starCount[0] = packet->starCount1;
        data->starCount[1] = packet->starCount2;
    }
    kmCall(0x80665310, ImportPatchRaceHeader1);

    void SetTrackVote(Pages::Vote& page){
        page.SetVotedCourseId((CourseId)Cosmos::CupManager::GetStaticInstance()->lastSelectedCourse);
        CosmosLog("Setting vote track to: %03x\n", Cosmos::CupManager::GetStaticInstance()->lastSelectedCourse);
    }
    kmCall(0x8084099c, SetTrackVote);

    u32 GetCorrectSlot(const RKNetRH1Handler& handler){
        Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();

        for(int i = 0; i < 12; i++){
            const RH1Data& data = handler.rh1Data[i];
            u32 track = data.trackId;
            if(track == 0xFFFFFFFF) continue;
            if(track >= 0x43 && track < 0xff) continue;
            if(data.timer == 0) continue;

            manager->SetWinningTrack(data.trackId);
            return manager->GetCurrentTrackSlot();
        }
        return 0xFFFFFFFF;
    }

    u8* GetPlayerAids(const RKNetRH1Handler& handler){
        for(int i = 0; i < 12; i++){
            const RH1Data& data = handler.rh1Data[i];
            u32 track = data.trackId;
            if(track == 0xFFFFFFFF) continue;
            if(track >= 0x43 && track < 0xff) continue;

            return (u8 *) data.aidsBelongToPlayer;
        }
        return nullptr;
    }
    kmBranch(0x80664b34, GetPlayerAids);

    bool IsTrackIDValid() {
        const RKNetRH1Handler* handler = RKNetRH1Handler::sInstance;
        for(int i = 0; i < 12; i++){
            const RH1Data& data = handler->rh1Data[i];
            u32 track = data.trackId;
            if(track == 0xFFFFFFFF) continue;
            if(track >= 0x43 && track < 0xff) continue;

            return true;
        }
        return false;
    }
    kmWrite32(0x80664084, 0x2C030000); //cmpwi r3, r0
    kmCall(0x806643a4, IsTrackIDValid);
    kmCall(0x80664080, IsTrackIDValid);
    
    /*
    Im gonna be 100% honest, ive spend way to long on this, had several issues related to missing patches,
    so Ive used Pulsar source code to find if there was something I was missing, and without it I wouldnt
    be able to finish it without going insane, shoutouts to Melg, his carrying this project, thats why this 
    piece of extension wont be that public and promoted, him and Pulsar should get all the clout;
    */


    kmWrite32(0x8066059c, 0xA0630018);
    kmWrite32(0x806605a0, 0x60000000); 
    kmWrite32(0x806605b8, 0xA0630050);
    kmWrite32(0x806605bc, 0x60000000); 
    kmWrite32(0x80661810, 0xA0180018);
    kmWrite32(0x806618e4, 0xA01C0050);
    kmWrite32(0x80661e90, 0xA01F0018);
    kmWrite32(0x80661ef0, 0xA0030050); 
    kmWrite32(0x80660de0, 0xA0030018);
    kmWrite32(0x8066141c, 0xA01C0018);

    kmWrite32(0x80660004, 0xB3540018);
    kmWrite32(0x80660140, 0xB3730050);

    kmWrite32(0x80660760, 0xB0C30018);

    kmWrite32(0x80660908, 0xB3B80018);


    kmWrite32(0x806440c0, 0x2c030100);
    kmWrite32(0x806440c8, 0x40a00020);
    kmWrite32(0x8064411c, 0x2c0300FF); 
    kmWrite32(0x80644150, 0x386000ff); 
    kmWrite32(0x80644154, 0x2c0300FF); 
    kmWrite32(0x80644338, 0x2C0300FF); 
    kmWrite32(0x8064433c, 0x418200dc);

    kmWrite32(0x80651374, 0x88030006); 
    kmWrite32(0x806600b0, 0x9813001E);
    kmWrite32(0x806600c0, 0x9B73001E);
    kmWrite32(0x806609b0, 0x981F001E);
    kmWrite32(0x806609c0, 0x9B9F001E);

    kmWrite32(0x80660e54, 0xA003003C); 
    kmWrite32(0x80660e58, 0x2c0000ff); 
    kmWrite32(0x80660b48, 0xB3D90010);
    kmWrite32(0x80660b5c, 0x388000ff);
    kmWrite32(0x80660b64, 0xB0980034);
    kmWrite32(0x80661480, 0xA01C003C);
    kmWrite32(0x80661484, 0x2c0000ff); 
    kmWrite32(0x80661648, 0xa078003c); 
    kmWrite32(0x8066164c, 0x2c0300ff); 
    kmWrite32(0x80661658, 0xA01C0074); 
    kmWrite32(0x80661754, 0xA018003C);
    kmWrite32(0x80661758, 0x2c0000ff);
    kmWrite32(0x8066184c, 0xA0DC0074);
    kmWrite32(0x80661854, 0x2c0600ff);
    kmWrite32(0x80661f0c, 0xA01F003C);
    kmWrite32(0x80661f10, 0x2c0000ff); 


    kmWrite32(0x80660018, 0x386000ff);
    kmWrite32(0x80660020, 0xB07F003C);
    kmWrite32(0x80660150, 0xB3D50074);

    kmWrite32(0x80661e94, 0xB01F003C);
    kmWrite32(0x80661ef4, 0xB01F003C);
    kmWrite32(0x80661f94, 0xB3DF003C);
    kmWrite32(0x8066200c, 0xB01F003C);
    kmWrite32(0x80660924, 0xB07F003C);

    kmWrite32(0x80661878, 0xB0D8003C);



#pragma region UglyAssDefines
    #define LoadPhase(addr) \
    asm int LoadPhase##addr() {\
        ASM (\
            nofralloc;\
            lbz r0, 0x3F (r3); \
            rlwinm r0, r0, 28, 28, 31; \
            blr; \
        ) \
        };\
    kmBranch(addr, LoadPhase##addr);\
    kmPatchExitPoint(LoadPhase##addr, addr+4);

    #define GetCorrectPhase(off, from, to) GetCorrectPhase##off##from##to() { \
        ASM (\
            nofralloc; \
            lbz to, off (from); \
            rlwinm to, to, 28, 28, 31; \
            blr; \
        ) \
    }

    #define SetCorrectPhase(off, from, to) SetCorrectPhase##off##from##to() { \
        ASM (\
            nofralloc;\
            lbz r12, off (from);\
            rlwimi r12, to, 4, 24, 27;\
            stb r12, off (from);\
            blr;\
        )\
    }

    #define GetEngine(off, to, from) \
    GetEngine##off##to##from() {\
        ASM (\
            nofralloc;\
            lbz to, off (from);\
            rlwinm to, to, 0, 28, 31;\
        )\
    }

    #define StoreEngine(off, to, from)\
    StoreEngine##off##to##from(){\
        ASM(\
            nofralloc;\
            lbz r12, off (from);\
            rlwimi r12, to, 0, 28, 31;\
            stb r12, off (from);\
            blr;\
        )};

#pragma endregion

    asm int GetCorrectPhase(0x3F, r24, r0);
    asm int GetCorrectPhase(0x77, r28, r0);

    kmCall(0x80661578, GetCorrectPhase0x3Fr24r0);
    kmCall(0x806617b8, GetCorrectPhase0x3Fr24r0);

    kmCall(0x80661880, GetCorrectPhase0x77r28r0);

    asm int SetCorrectPhase(0x3F,r24,r31);
    asm int SetCorrectPhase(0x3F,r24,r30);

    kmCall(0x806617a8, SetCorrectPhase0x3Fr24r31);
    kmCall(0x8066188c, SetCorrectPhase0x3Fr24r31);
    kmCall(0x80661908, SetCorrectPhase0x3Fr24r31);

    kmCall(0x8066163c, SetCorrectPhase0x3Fr24r30);

    LoadPhase(0x80660434);
    LoadPhase(0x80660470);
    LoadPhase(0x806604a8);
    LoadPhase(0x80660654);
    LoadPhase(0x80660710);

    kmWrite32(0x8066003c, 0x60000000);
    kmWrite32(0x80660168, 0x60000000);

    asm int GetEngine(0x77, r0, r28);
    kmCall(0x806615bc, GetEngine0x77r0r28);


    asm int GetEngine(0x3F, r3, r24);
    kmCall(0x806615b8, GetEngine0x3Fr3r24);

    asm int StoreEngine(0x3F, r28, r29);
    kmCall(0x80661cb8, StoreEngine0x3Fr28r29);

    asm int CheckCorrectEngineClass() {
        ASM (
            nofralloc;
            lbz r12, 0x3F (r24);
            lbz r0, 0x77 (r28);
            rlwimi r12, r0, 0, 28, 31;
            rlwinm. r0, r0, 28, 28, 31;
            beq skip;
            rlwimi r12, r30, 4, 24, 27;
        skip:
            stb r12, 0x3F (r24);
            li r0, 0;
            blr;
        )
    }
    kmCall(0x806617e8, CheckCorrectEngineClass);

} // namespace CosmosNetwork    
