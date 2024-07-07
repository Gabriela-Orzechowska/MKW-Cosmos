#ifndef _DWC_ //to split into multiple files ultimately
#define _DWC_
#include "types.hpp"
#include <core/gamespy/gamespy.hpp>
//Credit Seeky

static u64 wiiFriendCode; //80386328

namespace DWC{ //this is probably C, but don't care
struct LoginID{
    u64 userId;
    u32 playerId;
};

struct UserData{ //https://wiki.tockdom.com/wiki/Rksys.dat#DWC_User_Data
    u32 size;
    LoginID pseudoId;
    LoginID authenticID;
    int gsProfileId; //0x1c
    int flag;
    u32 gamecode; //always RMCJ
    u32 unknown_0x28[0x3c-0x28];
    u32 crc32;
};

struct Friend{
    u32 bitflag;
    u32 pid;
    u32 unknown_0x4;
};

struct FriendUpdate{
    u8 unknown[4];
    u32 pid;
};

struct DWCMatchContext {
    GameSpy::GPConnection* connection;
};
extern DWCMatchContext* stpMatchCnt;


}//namespace DWC


#endif
