#include "Ghost/GhostManager.hpp"
#include "System/identifiers.hpp"
#include "UI/Ctrl/PushButton.hpp"
#include "UI/Page/Other/TTSplits.hpp"
#include "UI/Page/Other/YesNo.hpp"
#include "kamek.hpp"
#include <Ghost/GhostUpload.hpp>

namespace Aurora {
    namespace Ghost {
        GhostLeaderboardAPI* GhostLeaderboardAPI::sInstance = nullptr;
        s32 GhostLeaderboardAPI::SendGhostData(RKG* rkg, u32 size, char* trackSha) {

            this->currentBuffer = rkg;
            this->currentSize = size;
            this->currentSha1 = trackSha;

            if(delay > currentCounter) return;


            RKNetController::GetStaticInstance()->StartMainLoop(0);
            RKSystem::mInstance.asyncDisplay->endFrame();
            RKSystem::mInstance.asyncDisplay->beginFrame();

            DWC::GHTTP::Init(nullptr);
            DWC::GHTTP::DWCGHTTPPost post;
            DWC::GHTTP::NewPost(&post);

            DWC::GHTTP::PostAddFileFromMemoryA(post, "ghost.bin", (void*)this->currentBuffer, this->currentSize, "ghost.bin", nullptr); 
            MenuData* menuData = MenuData::GetStaticInstance();
            GlobalContext* context = menuData->GetCurrentContext();


            char link[0x200];
            char miiName[0x30];
            memset(miiName, 0, 0x30);
            wcstombs(miiName, this->currentBuffer->header.miiData.miiName, 10);
            snprintf(link, 0x200, ghostUploadLink, this->currentSha1, miiName, this->currentBuffer->header.minutes, rkg->header.seconds, 
                this->currentBuffer->header.milliseconds, Cosmos::System::GetStaticInstance()->GetTTMode() == Cosmos::COSMOS_TT_200cc ? "200" : "150");
            s32 ret = DWC::GHTTP::PostData(link, &post, GhostLeaderboardAPI::SendGhostDataCallback, nullptr);
            if(ret >= 0) sendRequest = true;
            else CosmosLog("There has been and error creating the request! %d\n", ret);

            sendRequest = true;
            while(sendRequest){
                DWC::GHTTP::Process();
            };

            DWC::GHTTP::Shutdown();
        }

        void AddAPIDelay() {
            GhostLeaderboardAPI* api = GhostLeaderboardAPI::GetStaticInstance();
            if(api == nullptr) return;
            if(api->GetDelay() == 0) return;
            api->AddFrameToCounter();
        }
        static RaceFrameHook rfhAddAPIDelay(AddAPIDelay);

        void GhostLeaderboardAPI::SendGhostDataCallback(const char* buffer, u32 size, s32 ret, void* param){
            Scene* scene = MenuData::GetStaticInstance()->GetCurrentScene();
            Pages::AutoEnding* ending = scene->Get<Pages::AutoEnding>(TEXT_BOX_WITH_SPINNER);
            Pages::TTSplits* splits = scene->Get<Pages::TTSplits>(SPLITS_AFTER_TT);
            splits->EndStateAnimate(0, 0.0f);
            ending->EndStateAnimate(0.0f, 0);
            sInstance->sendRequest = false;
            sInstance->status = IDLE;
        };

        void GhostLeaderboardAPI::SetupVotePage() {
            Scene* scene = MenuData::GetStaticInstance()->GetCurrentScene();
            Pages::YesNoPopUp* page = scene->Get<Pages::YesNoPopUp>(VOTERANDOM_MESSAGE_BOX);
            page->SetMessageBoxMsg(0x2805, nullptr);
            page->SetupButton(0, 0x7d2, nullptr, 0, &this->onClickHandler);
            page->SetupButton(1, 0x7d3, nullptr, 0, &this->onClickHandler);
            this->status = ASKING;
        }
        void APIUploadRequest(void* holder) {
         ((GhostLeaderboardAPI*)holder)->SendGhostData();
        }

        void GhostLeaderboardAPI::OnButtonClick(u32 choice, PushButton& button) {
            Scene* scene = MenuData::GetStaticInstance()->GetCurrentScene();
            Pages::YesNoPopUp* page = scene->Get<Pages::YesNoPopUp>(VOTERANDOM_MESSAGE_BOX);
            page->EndStateAnimate(0, 0.0f);

            if(choice == 1) {
                Pages::TTSplits* splits = scene->Get<Pages::TTSplits>(SPLITS_AFTER_TT);
                splits->EndStateAnimate(0, 0.0);
                this->status = IDLE;
            }
            else {
                Pages::AutoEnding* ending = scene->Get<Pages::AutoEnding>(TEXT_BOX_WITH_SPINNER);
                ending->SetMessageWindowText(0x10cd, nullptr);
                page->AddPageLayer(TEXT_BOX_WITH_SPINNER, 0);
                Cosmos::Ghost::GhostManager* manager = Cosmos::Ghost::GhostManager::GetStaticInstance();

                GhostLeaderboardAPI::GetStaticInstance()->SetDelay(80);
                GhostLeaderboardAPI::GetStaticInstance()->SetData(&manager->rkg, manager->currentFileSize, Cosmos::System::GetStaticInstance()->GetTrackHash());
                GhostLeaderboardAPI::GetStaticInstance()->thread->Request(&APIUploadRequest, (void*)GhostLeaderboardAPI::GetStaticInstance(), NULL);
                this->status = BUSY;
            }
        }

        void PatchSplitOnFront(Pages::TTSplits& splits) {
            GhostLeaderboardAPI::GetStaticInstance()->SetupVotePage();
            splits.AddPageLayer(VOTERANDOM_MESSAGE_BOX, 0); 
            splits.ctrlRaceCount.zIndex = -100.0f;
            splits.ctrlRaceCount.SetMsgId(0,0);
        }
        kmBranch(0x80855854, PatchSplitOnFront);

        kmWrite32(0x80855adc, 0x4E800020);

        void CreateAPIInstance() {
            GhostLeaderboardAPI::CreateStaticInstance();
        }
        static BootHook bhCreateApiInstance(CreateAPIInstance, LOW);

    }
}
