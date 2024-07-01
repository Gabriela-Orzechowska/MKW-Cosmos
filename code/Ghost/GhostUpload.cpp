#include <Ghost/GhostUpload.hpp>

namespace Aurora {
    namespace Ghost {
        bool GhostLeaderboardAPI::sendRequest = false;

        s32 GhostLeaderboardAPI::SendGhostData(RKG* rkg, u32 size, char* trackSha) {
            RKNetController::GetStaticInstance()->StartMainLoop(0);

            RKSystem::mInstance.asyncDisplay->endFrame();
            RKSystem::mInstance.asyncDisplay->beginFrame();

            DWC::GHTTP::Init(nullptr);
            DWC::GHTTP::DWCGHTTPPost post;
            DWC::GHTTP::NewPost(&post);

            DWC::GHTTP::PostAddFileFromMemoryA(post, "ghost.bin", (void*)rkg, size, "ghost.bin", nullptr); 
            MenuData *menuData = MenuData::GetStaticInstance();
            GlobalContext *menu98 = menuData->GetCurrentContext();


            char link[0x200];
            char miiName[0x30];
            memset(miiName, 0, 0x30);
            wcstombs(miiName, rkg->header.miiData.miiName, 10);
            snprintf(link, 0x200, ghostUploadLink, trackSha, miiName, rkg->header.minutes, rkg->header.seconds, 
                rkg->header.milliseconds, Cosmos::System::GetStaticInstance()->GetTTMode() == Cosmos::COSMOS_TT_200cc ? "200" : "150");
            s32 ret = DWC::GHTTP::PostData(link, &post, GhostLeaderboardAPI::SendGhostDataCallback, nullptr);
            if(ret >= 0) sendRequest = true;
            else CosmosLog("There has been and error creating the request! %d\n", ret);

            sendRequest = true;
            while(sendRequest){
                DWC::GHTTP::Process();
            };

            DWC::GHTTP::Shutdown();
        }

        void GhostLeaderboardAPI::SendGhostDataCallback(const char* buffer, u32 size, s32 ret, void* param){
            sendRequest = false;
        };
    }
}
