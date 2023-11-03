#include <Debug/IOSDolphin.hpp>

namespace IOS
{
    namespace Dolphin
    {
        static char result[0x10];
        char * GetVersion()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return nullptr;
            
            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = result;
            request.size = 0x0F;

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_VERSION, 0, 1, &request);

            return result;
        }
        char * GetVersionName()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return nullptr;

            static char output[0x20];
            snprintf(output, 0x20, "Dolphin %s", GetVersion());
            return output;
        }
    }
}