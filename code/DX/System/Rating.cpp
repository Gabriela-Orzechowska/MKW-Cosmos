#include <DX/System/Rating.hpp>
#include <game/Network/RKNetUser.hpp>
#include <game/Network/RKNetController.hpp>
#include <Debug/IOSDolphin.hpp>

#define DIFF_SCALE
#define MINVR 0
#define MAXVR 35000

#define MINWIN 4
#define MAXWIN 25
#define MINLOSE 3
#define MAXLOSE 10

namespace DX
{
    s16 CalculatePoints(Rating * player, Rating * opponent, bool isWinning)
    {
        u16 playerPoints = player->mPoints;
        u16 opponentPoints = opponent->mPoints;

        s32 diff = (opponentPoints - playerPoints);

        if(diff < -MAXVR+1) diff = -MAXVR+1;
        if(diff > MAXVR-1) diff = MAXVR-1;
        if(isWinning) diff = -diff;
        diff = diff / 3;

        diff = diff + 9998;
        diff = diff / 5000;

        float val = CalcRating((float)(diff*0.00020004001f));

        if(isWinning)
        {
            if(val < MINWIN) val = MINWIN;
            if(val > MAXWIN) val = MAXWIN;
        }
        else
        {
            if(val < MINLOSE) val = MINLOSE;
            if(val > MAXLOSE) val = MAXLOSE;
            val = -val;
        }
        return (s16) val;
    }

    s32 CalcPosRating(Rating * player, Rating * opp)
    {
        return CalculatePoints(player,opp,true);
    }
    kmCall(0x8052eb1c, CalcPosRating);
    s32 CalcNegRating(Rating * player, Rating * opp)
    {
        return CalculatePoints(player,opp,false);
    }
    kmCall(0x8052eb5c, CalcNegRating);

    void RKNetUSERHandler_SendVrViaCity(RKNetUSERHandler * handler)
    {
        handler->BuildUserPacket();

        LicenseManager * license = &SaveDataManager::sInstance->licenses[SaveDataManager::sInstance->curLicenseId];

        handler->toSendPacket.vr = 5000;
        handler->toSendPacket.br = 5000;
        handler->toSendPacket.city = license->vr.mPoints;

        CosmosLog("USER Packet data: %p Handler at: %p\n", &handler->toSendPacket, handler);

        return;
    }

    kmCall(0x806628b0, RKNetUSERHandler_SendVrViaCity);

    void RKNetUSERHandler_UpdateVrViaCity(RKNetUSERHandler * handler)
    {
        LicenseManager * license = &SaveDataManager::sInstance->licenses[SaveDataManager::sInstance->curLicenseId];

        handler->toSendPacket.vr = 5000;
        handler->toSendPacket.br = 5000;
        handler->toSendPacket.city = license->vr.mPoints;

        CosmosLog("USER Packet data: %p Handler at %p\n", &handler->toSendPacket, handler);
    }

    kmCall(0x806602d0, RKNetUSERHandler_UpdateVrViaCity);


    kmWrite32(0x8060a1f4, 0xa004017a); //lhz r0,0x17a(r4) receivedPackets[i]->city; 
    kmWrite32(0x8065fcdc, 0xa0a3017a); //lhz r5,0x17a(r3) receivedPackets[i]->city;
    kmWrite32(0x805d302c, 0xa063017a); //lhz r3,0x17a(r3) receivedPackets[param_2]->city;
    kmWrite32(0x80651a30, 0xa003017A); //lhz r0,0x17a(r3) receivedPackets[i]->city;
    kmWrite32(0x80661b24, 0xa006017A); //lhz r0,0x17a(r3) receivedPackets[1]->city;
    kmWrite32(0x80661b94, 0xa006023A); //lhz r0,0x23a(r3) receivedPackets[2]->city;
    kmWrite16(0x805d3032, 0x88B8); //Bump Max to 35000
    kmWrite16(0x805d303A, 0x88B8); //Bump Max to 35000
    kmWrite16(0x80659342, 0x88B8); //Bump Max to 35000
    kmWrite16(0x8065934A, 0x88B8); //Bump Max to 35000


    extern "C"{
        int DWC_AddMatchKeyInt(u32 id, char * param_2, u32 * valuePointer);
    }

    s32 PatchDWCVR(u32 id, char * param_2, u32 * valuePointer)
    {
        RKNetController::sInstance->vr = 5000;
        RKNetController::sInstance->br = 5000;
        return DWC_AddMatchKeyInt(id, param_2, valuePointer);
    }
    kmCall(0x80659524, PatchDWCVR);
    kmCall(0x80659834, PatchDWCVR);
    kmWrite32(0x8065fc7c, 0x3ca01388);
    kmWrite32(0x80661b1c, 0x38001388);
    kmWrite32(0x80661b8c, 0x38001388);
} // namespace DX
