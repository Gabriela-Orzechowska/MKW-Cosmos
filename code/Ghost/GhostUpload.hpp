#include "core/System/RKSystem.hpp"
#include <kamek.hpp>
#include <game/File/RKG.hpp>
#include <core/rvl/DWC/GHTTP.hpp>
#include <game/Race/RaceData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Other/YesNo.hpp>
#include <game/Network/RKNetController.hpp>
#include <main.hpp>

namespace Aurora {
    namespace Ghost {
        const char ghostUploadLink[] = "http://cosmos.gabriela-orzechowska.com/aurora/api/upload-rkg/%s?name=%s&time=%01dm%02ds%03d&mode=%s";

        class GhostLeaderboardAPI {
        public:
            enum Status {
                IDLE,
                ASKING,
                BUSY,
                UNAVAILABLE,
            };
            GhostLeaderboardAPI() : status(IDLE) { }

            static inline GhostLeaderboardAPI* GetStaticInstance() { return sInstance; }
            static void CreateStaticInstance() { if(!sInstance) sInstance = new GhostLeaderboardAPI; }
            s32 SendGhostData(RKG* buffer, u32 bufferSize, char* sha1);
        private:
            static GhostLeaderboardAPI* sInstance;

            static void SendGhostDataCallback(const char* buffer, u32 size, s32 ret, void* param);
            bool sendRequest;
            Status status;
        };
    }
}


