#include <System/Scene.hpp>

namespace Cosmos
{
    namespace Scene
    {
        bool PushDolphinSpeed(u32 percent)
        {
            if(!IOS::Dolphin::IsOpen()) return false;
            if(v_dolSpeedSize == 7) {
                CosmosLog("Max Stack Size reached\n");
                return false;
            }

            u32 curLimit = IOS::Dolphin::GetCPUSpeed();
            if(curLimit == 0) {
                CosmosLog("Unable to receive current CPU speed\n");
                return false;
            }
            if(IOS::Dolphin::SetSpeedLimit(percent) >= 0)
            {
                v_dolSpeedTab[v_dolSpeedSize++] = curLimit;
            }
            else
            {
                CosmosLog("Failed to set speed\n");
                return false;
            }
            return true;
        }
        
        bool PopDolphinSpeed()
        {
            if(!IOS::Dolphin::IsOpen()) return false;
            if(v_dolSpeedSize > 0){
                return IOS::Dolphin::SetSpeedLimit(v_dolSpeedTab[--v_dolSpeedSize]) >= 0;
            }
            return false;
        }


    } // namespace Scene
    
} // namespace Cosmos
