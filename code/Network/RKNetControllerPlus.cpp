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
    

} // namespace CosmosNetwork
