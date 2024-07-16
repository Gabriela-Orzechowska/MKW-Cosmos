#include "kamek.hpp"
#include "main.hpp"
#include <Debug/Debug.hpp>

#define vWIIMENU 0x0000000100000200


namespace CosmosDebug
{
    void DetectPlatform()
    {
        if(IOS::Dolphin::Open() >= 0)
        {
            currentPlatform = DOLPHIN;
            if(!IOS::Dolphin::GetVersion()) currentPlatform = DOLPHIN_OLD;
            IOS::Dolphin::Close();
            return;
        }

        register u32 cpuid;
        asm{
            ASM(
                mfspr cpuid, 0x39c;
            )
        };
        if(cpuid == DolphinECID_U){
            currentPlatform = DOLPHIN_UNKNOWN;
            return;
        }
        else if(cpuid == 0) {
            currentPlatform = DOLPHIN_PREHISTORIC;
            return;
        }

        IOS::IPCResult ret = Cosmos::Open("/title/00000001/00000002/data/macaddr.bin", IOS::MODE_READ);
        if(ret == IOS::IPC_OK)
        {
            IOS::Close(ret);
            currentPlatform = WII_MINI;
            return;
        }
        if(OSIsTitleInstalled(vWIIMENU))
        {
            currentPlatform = WII_U;
        }
        else
        {
            currentPlatform = WII;
        }
        return;
    }

    char* GetPlatformName() { return platformName; }

    void UpdatePlatformString()
    {
        DetectPlatform(); 
        char* platformString;
        switch(currentPlatform)
        {
            case DOLPHIN:
                platformString = IOS::Dolphin::GetVersionName();
                break;
            case DOLPHIN_OLD:
                platformString = "Dolphin (OUTDATED)";
                break;
            case WII_MINI:
                platformString = "Wii Mini";
                break;
            case WII_U:
                platformString = "Wii U";
                break;
            case WII:
                platformString = "Wii";
                break;
        }
        strncpy(platformName, platformString, 0x20);
    }

    static BootHook SetupDebug(UpdatePlatformString, MEDIUM);
}
