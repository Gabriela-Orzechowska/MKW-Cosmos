#include <System/Rating.hpp>

#define DIFF_SCALE
#define MINVR 0
#define MAXVR 35000

#define MINWIN 4
#define MAXWIN 25
#define MINLOSE 3
#define MAXLOSE 10

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