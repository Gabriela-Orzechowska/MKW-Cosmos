#include "core/System/RKSystem.hpp"
#include "core/egg/Thread.hpp"
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
        
        enum Mode {
            MODE_150,
            MODE_200,
        };

#pragma pack(push, 1)
        struct UploadData {
            u32 checkNum;
            u32 timems;
            Mode mode;
            u32 padding;
            u64 sessionToken;
            wchar_t miiName[0x10];
        };  
#pragma pack(pop)

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
            static void CreateStaticInstance() { if(!sInstance) { 
                sInstance = new GhostLeaderboardAPI;
                sInstance->thread = EGG::TaskThread::Create(2, 25, 0x2000, nullptr); 
            }}
            
            s32 SendGhostData() { return SendGhostData(this->currentBuffer, this->currentSize, this->currentSha1); }
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
            void SetData(RKG* rkg, u32 size, char* trackSha) {
                CosmosLog("Setting Data: %p\n", rkg);
                this->currentBuffer = rkg;
                this->currentSize = size;
                this->currentSha1 = trackSha;
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
            EGG::TaskThread* thread;
        };
    }
}


