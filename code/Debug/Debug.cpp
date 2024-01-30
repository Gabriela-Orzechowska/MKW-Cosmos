#include <Debug/Debug.hpp>

#define vWIIMENU 0x0000000100000200


namespace CosmosDebug
{
    void DetectPlatform()
    {
        if(IOS::Dolphin::Open())
        {
            currentPlatform = DOLPHIN;
            if(!IOS::Dolphin::GetVersion()) currentPlatform = DOLPHIN_OLD;
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

    char * GetPlatformString()
    {
        switch(currentPlatform)
        {
            case DOLPHIN:
                return IOS::Dolphin::GetVersionName();
            case DOLPHIN_OLD:
                return "Dolphin (OUTDATED)";
            case WII_MINI:
                return "Wii Mini";
            case WII_U:
                return "Wii U";
            case WII:
                return "Wii";
        }
        Cosmos::Panic(__FILE__, __LINE__, "Platform cannot be NULL.");
        return nullptr;
    }
    void UpdatePlatformString()
    {
        DetectPlatform(); 
        strncpy(platformName, GetPlatformString(), 0x20);
    }

    static BootHook SetupDebug(UpdatePlatformString, HIGH);
}