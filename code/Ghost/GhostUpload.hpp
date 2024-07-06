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
                UPLOADED,
                UNAVAILABLE,
            };
            GhostLeaderboardAPI() : status(IDLE), delay(0), currentCounter(0) { 
                SetupHandler(onClickHandler, void (GhostLeaderboardAPI::*)(u32, PushButton&), &GhostLeaderboardAPI::OnButtonClick); 
            }

            static inline GhostLeaderboardAPI* GetStaticInstance() { return sInstance; }
            static void CreateStaticInstance() { if(!sInstance) sInstance = new GhostLeaderboardAPI; }
            s32 SendGhostData(RKG* buffer, u32 bufferSize, char* sha1);
            Status GetStatus() { return status; }
            u32 GetDelay() { return delay; }
            void SetDelay(u32 frames) { delay = frames; currentCounter = 0; }

            void AddFrameToCounter() { currentCounter++; if (delay >= 0 && delay <= currentCounter) {
                    this->SendGhostData(this->currentBuffer, this->currentSize, this->currentSha1);
                    this->delay = -1;
                    this->currentCounter = 0;
                }
            }


            void SetupVotePage();

        private:
            static GhostLeaderboardAPI* sInstance;

            static void SendGhostDataCallback(const char* buffer, u32 size, s32 ret, void* param);
            bool sendRequest;
            Status status;

            u32 delay;
            u32 currentCounter;

            RKG* currentBuffer;
            u32 currentSize;
            char* currentSha1;

            void OnButtonClick(u32 choice, PushButton& button);

            PtmfHolder_2A<GhostLeaderboardAPI, void, u32, PushButton&> onClickHandler;
        };
    }
}


