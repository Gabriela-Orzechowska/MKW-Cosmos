#include <Network/RKNetControllerPlus.hpp>

namespace CosmosNetwork
{
    u8 GetActualEngineClass(RKNetSELECTHandlerPlus * handler) {
        if(handler->toSendPacket.GetPhase() != 0) return handler->toSendPacket.GetEngineClass();
        return 0xFF;
    }
    kmBranch(0x8066048c, GetActualEngineClass);

    u16 GetActualWinningCourse(RKNetSELECTHandlerPlus * handler) {
        if(handler->toSendPacket.GetPhase() == 2) return handler->toSendPacket.winningCourse;
        return 0xFF;
    }
    kmBranch(0x80660450, GetActualWinningCourse);

    bool HasDecided(RKNetSELECTHandlerPlus * handler) {
        return handler->toSendPacket.winningCourse != 0xFF;
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
        
        bool hostAlwaysWin = false; //Cosmos::Data::SettingsHolder::GetInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_HOST_ALWAYS_WINS) == Cosmos::Data::ENABLED;

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
            handler.toSendPacket.winningCourse = actualVote;
            handler.toSendPacket.winningVoterAid = winnerAid;
            manager->AddTrackToBlocking(actualVote);
        }
        else ((RKNetSELECTHandler*)&handler)->DecideTrack();
    }
    kmCall(0x80661490, DecideTrack);

    u32 SetCorrectSlot(RKNetSELECTHandlerPlus& handler) {
        if(handler.toSendPacket.GetPhase() == 2) return Cosmos::CupManager::GetStaticInstance()->GetCurrentTrackSlot();
        return -1;
    }
    kmCall(0x80650ea8, SetCorrectSlot);

    void LoadCorrectTrack(ArchiveRoot& root, u32 winningCourse){
        Cosmos::CupManager::GetStaticInstance()->SetWinningTrack(winningCourse);
        root.RequestLoadCourseAsync((CourseId)winningCourse);
    }
    kmCall(0x80644414, LoadCorrectTrack);
    
    /*
    Im gonna be 100% honest, ive spend way to long on this, had several issues related to missing patches,
    so Ive used Pulsar source code to find if there was something I was missing, and without it I wouldnt
    be able to finish it without going insane, shoutouts to Melg, his carrying this project, thats why this 
    piece of extension wont be that public and promoted, him and Pulsar should get all the clout;
    */

    //Patch winningCourse u8 > u16

    // lhz r0, 0x3c(r3); nop; cmpwi r0, 0xFF
    kmWrite32(0x80660e50, 0xA003003C);
    kmWrite32(0x80660e54, 0x60000000);
    kmWrite32(0x80660e58, 0x2C0000FF);

    // lhz r0, 0x3c(r28); nop; cmpwi r0, 0xFF
    kmWrite32(0x8066147c, 0xA01C003C);
    kmWrite32(0x80661480, 0x60000000);
    kmWrite32(0x80661464, 0x2C0000FF);

    // lhz r0, 0x3c(r24); nop; cmpwi r0, 0xFF
    kmWrite32(0x80661644, 0xA018003C);
    kmWrite32(0x80661648, 0x60000000);
    kmWrite32(0x8066164C, 0x2C0000FF);

    // lhz r0, 0x74(r28); nop; cmpwi r0, 0xFF
    kmWrite32(0x80661654, 0xA01C0074);
    kmWrite32(0x80661658, 0x60000000);
    kmWrite32(0x8066165C, 0x2C0000FF);

    // lhz r0, 0x3C(r31); nop; cmpwi r0, 0xFF
    kmWrite32(0x80661f08, 0xA01F003C);
    kmWrite32(0x80661f0C, 0x60000000);
    kmWrite32(0x80661f10, 0x2C0000FF);
    
    // lhz r0, 0x3C(r31); nop; cmpwi r0, 0xFF
    kmWrite32(0x80661750, 0xA018003C);
    kmWrite32(0x80661754, 0x60000000);
    kmWrite32(0x80661758, 0x2C0000FF);

    // lhz r0, 0x3C(r31); nop; cmpwi r0, 0xFF
    kmWrite32(0x80661840, 0xA0DC0074);
    kmWrite32(0x8066184c, 0x60000000);
    kmWrite32(0x80661854, 0x2C0000FF);

    kmWrite32(0x80660018, 0x386000FF);
    kmWrite32(0x80660020, 0xB07F003C);
    kmWrite32(0x80660150, 0xB3D50074);
    kmWrite32(0x80660b48, 0xB3D90010);
    kmWrite32(0x80660b5c, 0x388000FF);
    kmWrite32(0x80660b64, 0xB0980034);
    kmWrite32(0x80660924, 0xB07F003C);
    kmWrite32(0x80661878, 0xB0D8003C);
    kmWrite32(0x80661e94, 0xB01F003C);
    kmWrite32(0x80661ef4, 0xB01F003C);
    kmWrite32(0x80661f94, 0xB3DF003C);
    kmWrite32(0x8066200c, 0xB01F003C);

    kmWrite32(0x806440c0, 0x2c030100); 
    kmWrite32(0x806440c8, 0x40a00020);

    // Swap rank and vote

    kmWrite32(0x8066059c, 0xA0630018);
    kmWrite32(0x806605a0, 0x60000000);

    kmWrite32(0x806605b8, 0xA0630050);
    kmWrite32(0x806605bc, 0x60000000);

    kmWrite32(0x80660de0, 0xA0030018);
    kmWrite32(0x8066141c, 0xA01C0018);

    kmWrite32(0x80661810, 0xA0180018);
    kmWrite32(0x806618e4, 0xA01C0050);

    kmWrite32(0x80661e8C, 0xA01F0018);
    kmWrite16(0x80661e90, 0x60000000);
    kmWrite32(0x80661ef0, 0xA0030050); 

    kmWrite32(0x80660004, 0xB3540018);
    kmWrite32(0x80660140, 0xB3730050);
    kmWrite32(0x80660760, 0xB0C30018);
    kmWrite32(0x80660908, 0xB3B80018);

    kmWrite32(0x80651374, 0x88030006);
    kmWrite32(0x806600b0, 0x9813001E);
    kmWrite32(0x806600c0, 0x9B73001E);
    kmWrite32(0x806609b0, 0x981F001E);
    kmWrite32(0x806609c0, 0x9B9F001E);

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
    kmPatchExitPoint(LoadPhase##addr,addr+4);

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
            rlwimi r12, to, 5, 24, 27;\
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

    kmCall(0x806617a8, SetCorrectPhase0x3Fr24r31);
    kmCall(0x8066188c, SetCorrectPhase0x3Fr24r31);
    kmCall(0x80661908, SetCorrectPhase0x3Fr24r31);

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
