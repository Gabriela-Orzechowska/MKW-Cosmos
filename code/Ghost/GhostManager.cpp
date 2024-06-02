#include <Ghost/GhostManager.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <game/UI/Page/RaceMenu/TTPause.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>
#include <Debug/Draw/DebugDraw.hpp>
#include <Settings/UserData.hpp>

void CorrectGhostTrackName(LayoutUIControl *control, const char *textBoxName, u32 bmgId, const TextInfo *text)
{
    u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
    bmgId = trackId + 0x7000;
    control->SetTextBoxMsg(textBoxName, bmgId, text);
    return;
}

kmCall(0x805e2a4c, CorrectGhostTrackName);

namespace Cosmos
{
    namespace Ghost
    {
        GhostManager *GhostManager::sInstance = NULL;
        char GhostManager::folderPath[IPCMAXPATH] = "";

        GhostManager::GhostManager()
        {
            this->folderManager = CosmosFile::FolderManager::Create();
            this->courseId = -1;
            this->files = NULL;
        }

        GhostManager::~GhostManager()
        {
            delete (this->folderManager);
        }

        GhostManager *GhostManager::CreateStaticInstance()
        {
            GhostManager *holder = GhostManager::sInstance;
            if (holder == nullptr)
            {
                holder = new (RKSystem::mInstance.EGGSystem) GhostManager;
                GhostManager::sInstance = holder;
            }
            holder->Reset();
#ifdef COSMOS_ANTI_CHEAT
            AntiCheat::CreateStaticInstance();
#endif
            return holder;
        }

        void GhostManager::DestroyStaticInstance()
        {
            if (GhostManager::sInstance != nullptr)
                delete (GhostManager::sInstance);
            GhostManager::sInstance = nullptr;
#ifdef COSMOS_ANTI_CHEAT
            AntiCheat::DestroyStaticInstance();
#endif
        }

        const char *ttFolders[2] = {
            "150",
            "200",
        };

        void GhostManager::Init(u32 courseId)
        {
            this->Reset();
            this->courseId = courseId;
            CosmosFile::FolderManager *manager = this->folderManager;

            snprintf(folderPath, IPCMAXPATH, "%s/%03x", Cosmos::ghostFolder, courseId);
            CosmosFile::FileManager::GetStaticInstance()->CreateFolder(folderPath);

            char folderModePath[IPCMAXPATH];
            snprintf(folderModePath, IPCMAXPATH, "%s/%s", folderPath, ttFolders[Cosmos::System::GetStaticInstance()->GetTTMode()]);
            CosmosFile::FileManager::GetStaticInstance()->CreateFolder(folderModePath);

            manager->ReadFolder(folderModePath);

            new (&this->GetLeaderboard()) GhostLeaderboardManager(folderPath, courseId);

            this->files = new (RKSystem::mInstance.EGGSystem) GhostData[manager->GetFileCount()];
            RKG *rkg = &this->rkg;
            u32 counter = 0;
            for (int i = 0; i < manager->GetFileCount(); i++)
            {
                rkg->ClearBuffer();
                GhostData *header = &this->files[counter];
                header->isValid = false;
                if (manager->ReadFile(rkg, i, CosmosFile::FILE_MODE_READ) > 0 && rkg->CheckValidity())
                {
                    header->Init(rkg);
                    counter++;
                }
                header->padding = i;
                manager->CloseFile();
            }
            this->rkgCount = counter;

            // Set correct CC mode
            EngineClass cc = CC_150;
            if (Cosmos::System::GetStaticInstance()->GetTTMode() == Cosmos::COSMOS_TT_200cc)
                cc = CC_100;
            RaceData::GetStaticInstance()->menusScenario.GetSettings().engineClass = cc;
        }

        void GhostManager::Reset()
        {
            this->courseId = -1;
            mainGhostIndex = 0xFF;
            delete[] this->files;
            this->files = nullptr;
            RaceData::GetStaticInstance()->menusScenario.GetPlayer(1).playerType = PLAYER_NONE;
        }

        bool GhostManager::LoadGhost(RKG *rkg, u32 index)
        {
            rkg->ClearBuffer();
            this->folderManager->ReadFile(rkg, index, CosmosFile::FILE_MODE_READ);
            return rkg->CheckValidity();
        }

        void GhostManager::LoadGhostReplay(RKG *buffer, bool isGhostRace)
        {
            if (this->mainGhostIndex != 0xFF)
            {
                if (this->LoadGhost(buffer, this->GetGhostData(this->mainGhostIndex)->padding))
                {
                    RaceData *raceData = RaceData::GetStaticInstance();
                    RKG *dest = &raceData->GetGhost(0);
                    if (buffer->header.compressed)
                        buffer->DecompressTo(dest);
                    else
                        memcpy(dest, buffer, sizeof(RKG));
                    raceData->menusScenario.GetPlayer(isGhostRace).playerType = PLAYER_GHOST;
                    GhostData data(dest);
                    MenuData::GetStaticInstance()->GetCurrentContext()->playerMiis.AddMii(isGhostRace, &data.miiData);
                }
            }
        }

        bool GhostManager::EnableGhost(const GhostListEntry &entry)
        {
            bool ret = false;
            for (int i = 0; i < this->rkgCount; i++)
            {
                if (&this->files[i] == entry.data)
                {
                    this->mainGhostIndex = i;
                    ret = true;
                    break;
                }
            }
            return ret;
        }

        void GhostManager::CreateAndSaveFiles(void *holder)
        {
            char path[IPCMAXPATH];
            GhostManager *manager = (GhostManager *)holder;
            RKG *rkg = &manager->rkg;
            snprintf(path, IPCMAXPATH, "%s/%05d-%01dm%02ds%03d.rkg", manager->folderManager->GetName(), OSGetTime(), rkg->header.minutes, rkg->header.seconds, rkg->header.milliseconds);
            CosmosFile::FileManager *fileManager = CosmosFile::FileManager::GetStaticInstance();
            fileManager->CreateOpen(path, CosmosFile::FILE_MODE_WRITE);
            u32 size = sizeof(RKG);
            if (rkg->header.compressed)
                size = ((CompressedRKG *)rkg)->dataLength + sizeof(RKGHeader) + 0x4 + 0x4;
            fileManager->Overwrite(size, rkg);
            fileManager->Close();

            char folderPath[IPCMAXPATH];
            snprintf(folderPath, IPCMAXPATH, "%s/%03x", Cosmos::ghostFolder, manager->courseId);
            manager->GetLeaderboard().Save(folderPath);
            manager->Init(Cosmos::CupManager::GetStaticInstance()->GetTrackID());

            manager->mainGhostIndex = manager->rkgCount - 1;
            MenuData::GetStaticInstance()->GetCurrentContext()->isNewTime = true;
        }
        static CosmosDebug::DebugMessage ghVerifyMessage(false);
        void GhostManager::VerifyTime()
        {
#ifdef COSMOS_ANTI_CHEAT
            if(!AntiCheat::GetStaticInstance()->IsRunValid()) return;

            AntiCheat::GetStaticInstance()->Update(RaceInfo::GetStaticInstance()->timer);

            if (!AntiCheat::GetStaticInstance()->IsRunValid())
            {
                Pages::RaceHUD *page = MenuData::GetStaticInstance()->curScene->Get<Pages::RaceHUD>(TIME_TRIAL_INTERFACE);
                if (page)
                {
                    page->ghostMessage->isHidden = false;
                    page->ghostMessage->SetMsgId(0x2802);
                }
                s32 delta = AntiCheat::GetStaticInstance()->GetTimeDelta();
                char msg[0x40];
                snprintf(msg, 0x40, "Delta:%dms", delta);
                ghVerifyMessage.SetMessage(msg);
                ghVerifyMessage.DisplayForX(120);
            }
#endif
        }
#ifdef COSMOS_ANTI_CHEAT

        void UpdateStartTime(Page &page, u32 soundIdx, u32 param_3)
        {
            AntiCheat* antiCheat = AntiCheat::GetStaticInstance();
            if(antiCheat != nullptr)
                antiCheat->Reset(IOS::Dolphin::GetSystemTime());
            GhostManager* manager = GhostManager::GetStaticInstance();
            if(manager != nullptr)
                manager->wereGhostsDisabled = Cosmos::Data::SettingsHolder::GetInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_GHOST_SAVING) == Cosmos::Data::DISABLED;
            page.PlaySound(soundIdx, param_3);
            return;
        }
        kmCall(0x80857790, UpdateStartTime);

        void VerifyTimeDuringRace()
        {
            Pages::RaceHUD *page = Pages::RaceHUD::sInstance;
            if (page)
            {
                if(Cosmos::Security::GeckoAnalizer::AreCheatsEnabled()){
                    page->ghostMessage->isHidden = false;
                    page->ghostMessage->SetMsgId(0x2804);
                    return;
                }
                if(Cosmos::Data::SettingsHolder::GetInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_GHOST_SAVING) == Cosmos::Data::DISABLED){
                    page->ghostMessage->isHidden = false;
                    page->ghostMessage->SetMsgId(0x2803);
                    return;
                }
            }

            if(RaceInfo::GetStaticInstance()->timer < 250) return;
            if (RaceData::GetStaticInstance()->racesScenario.GetSettings().gamemode == MODE_TIME_TRIAL) {
                GhostManager::GetStaticInstance()->VerifyTime();
            }
            
        }

        static RaceFrameHook rfhVerify(VerifyTimeDuringRace);

#endif

        GhostLeaderboardManager::GhostLeaderboardManager()
        {
            memset(&this->file, 0, sizeof(GhostLeaderboardManager));
            this->file.signature = 0x44584C44;
            this->file.version = 0x10;
            for (int i = 0; i < GAMEMODES; i++)
                this->file.ghostStatus[i] = 0x0;
        }

        GhostLeaderboardManager::GhostLeaderboardManager(const char *folderPath, u32 id)
        {
            new (this) GhostLeaderboardManager();
            char filePath[IPCMAXPATH];
            snprintf(filePath, IPCMAXPATH, "%s/ld.glm", folderPath);
            strncpy(this->folderPath, folderPath, IPCMAXPATH);
            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetStaticInstance();
            s32 ret = manager->Open(filePath, CosmosFile::FILE_MODE_READ);
            if (ret > 0)
                ret = manager->Read(&this->file, sizeof(GhostLeaderboardFile));
            if (ret <= 0)
            {
                GhostLeaderboardManager::CreateFile(id);
            }

            manager->Close();
        }

        void GhostLeaderboardManager::CreateFile(u32 id)
        {
            char filePath[IPCMAXPATH];
            snprintf(filePath, IPCMAXPATH, "%s/ld.glm", GhostManager::folderPath);
            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetStaticInstance();
            manager->CreateOpen(filePath, CosmosFile::FILE_MODE_READ_WRITE);
            GhostLeaderboardFile *file = new (RKSystem::mInstance.EGGSystem, 0x20) GhostLeaderboardFile;
            memset(file, 0, sizeof(GhostLeaderboardFile));
            file->trackId = id;
            manager->Overwrite(sizeof(GhostLeaderboardFile), file);
            manager->Close();
            delete (file);
        }

        void GhostLeaderboardManager::Update(s32 position, TimeEntry &entry, u32 id)
        {
            Cosmos::TT_MODE mode = Cosmos::System::GetStaticInstance()->GetTTMode();
            GhostLeaderboardFile &lfile = this->file;
            GhostTimeEntry &tentry = lfile.entry[mode][position];
            if (position != ENTRY_FLAP)
            {
                for (int i = ENTRY_5TH; i > position; i--)
                {
                    memcpy(&lfile.entry[mode][i], &lfile.entry[mode][i - 1], sizeof(GhostTimeEntry));
                }
            }
            memcpy(&tentry.mii, &entry.mii, sizeof(RawMii));
            tentry.minutes = entry.timer.minutes;
            tentry.seconds = entry.timer.seconds;
            tentry.miliseconds = entry.timer.milliseconds;
            tentry.isActive = entry.timer.isActive;
            tentry.controllerType = entry.controllerType;
            tentry.kart = entry.kart;
            tentry.character = entry.character;
        }

        void GhostLeaderboardManager::Save()
        {
            GhostLeaderboardManager::Save(this->folderPath);
        }

        s32 GhostLeaderboardManager::GetLeaderboardPosition(const Timer &timer) const
        {
#ifdef COSMOS_ANTI_CHEAT

            if (!AntiCheat::GetStaticInstance()->IsRunValid())
                return -1;

#endif

            s32 position = -1;
            Timer t_timer;
            for (int i = ENTRY_5TH; i >= 0; i--)
            {
                this->GhostTimeEntryToTimer(t_timer, i);
                if (t_timer > timer)
                    position = i;
            }
            return position;
        }

        void GhostLeaderboardManager::Save(const char *folderPath)
        {
            char filePath[IPCMAXPATH];
            snprintf(filePath, IPCMAXPATH, "%s/ld.glm", folderPath);
            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetStaticInstance();
            manager->Open(filePath, CosmosFile::FILE_MODE_WRITE);
            manager->Overwrite(sizeof(GhostLeaderboardFile), &this->file);
            manager->Close();
        }

        void GhostLeaderboardManager::GhostTimeEntryToTimer(Timer &timer, u32 index) const
        {
            Cosmos::TT_MODE mode = Cosmos::System::GetStaticInstance()->GetTTMode();
            timer.minutes = this->file.entry[mode][index].minutes;
            timer.seconds = this->file.entry[mode][index].seconds;
            timer.milliseconds = this->file.entry[mode][index].miliseconds;
            timer.isActive = this->file.entry[mode][index].isActive;
        }

        void GhostLeaderboardManager::GhostTimeEntryToTimeEntry(TimeEntry &entry, u32 index)
        {
            this->GhostTimeEntryToTimer(entry.timer, index);
            Cosmos::TT_MODE mode = Cosmos::System::GetStaticInstance()->GetTTMode();
            memcpy(&entry.mii, &this->file.entry[mode][index].mii, sizeof(RawMii));
            entry.character = this->file.entry[mode][index].character;
            entry.kart = this->file.entry[mode][index].kart;
            entry.controllerType = this->file.entry[mode][index].controllerType;
        }

        GhostLeaderboardFile::GhostLeaderboardFile()
        {
            this->signature = 0x44584C44;
            this->version = 0x10;
            for (int i = 0; i < GAMEMODES; i++)
                this->ghostStatus[i] = 0x0;
        }

        s32 PlayCorrectMusic(LicenseManager &license, Timer &timer, u32 courseId)
        {
#ifdef COSMOS_ANTI_CHEAT

            AntiCheat::GetStaticInstance()->CheckValidness();

#endif

            return GhostManager::GetStaticInstance()->GetLeaderboard().GetLeaderboardPosition(timer);
        }
        kmCall(0x80856fec, PlayCorrectMusic);

        TimeEntry *GetTimeEntry(u32 param_1, u32 index)
        {
            GhostManager *manager = GhostManager::GetStaticInstance();
            manager->GetLeaderboard().GhostTimeEntryToTimeEntry(manager->entry, index);
            return &manager->entry;
        }

        kmCall(0x8085d8d0, GetTimeEntry);
        kmCall(0x8085d5c8, GetTimeEntry);
        kmCall(0x8085da54, GetTimeEntry);

        void CustomGhostGroup(GhostList *list, u32 id)
        {
            u32 trackID = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
            GhostManager *manager = GhostManager::GetStaticInstance();
            manager->Init(trackID);
            u32 index = 0;
            for (int i = 0; i < manager->folderManager->GetFileCount(); i++)
            {
                if (index == 38)
                    break;
                const GhostData *data = manager->GetGhostData(i);
                if (data->isValid)
                {
                    list->entries[index].data = data;
                    index++;
                }
            }
            list->count = index;
            qsort(list, list->count, sizeof(GhostListEntry), (int (*)(const void *, const void *)) * &GhostList::CompareEntries);
        };
        kmCall(0x806394f0, CustomGhostGroup);

        void *CreatePageAndManager()
        {
            GhostManager::CreateStaticInstance();
            return new (Pages::GhostSelect);
        }
        kmCall(0x80623dec, CreatePageAndManager);
        kmWrite32(0x80623de0, 0x60000000);

        void DestroyManager(Scene *scene, PageId pageId)
        {
            scene->CreatePage(pageId);
            GhostManager::DestroyStaticInstance();
        }
        kmCall(0x8062cf98, DestroyManager);

        void ExtendSetupGhostRace(Pages::GhostManager *ghostManager, bool isStaffGhost, bool replaceGhostMiiByPlayer, bool disablePlayerMii)
        {
            ghostManager->SetupGhostRace(true, replaceGhostMiiByPlayer, disablePlayerMii);
            GhostManager::GetStaticInstance()->LoadGhostReplay(ghostManager->rkgPointer, true);
        }
        kmCall(0x805e13ac, ExtendSetupGhostRace);
        kmCall(0x805e13e4, ExtendSetupGhostRace);
        kmCall(0x805e141c, ExtendSetupGhostRace);
        kmCall(0x805e149c, ExtendSetupGhostRace);
        kmCall(0x805e14c8, ExtendSetupGhostRace);
        kmCall(0x805e14f4, ExtendSetupGhostRace);

        void LoadCorrectGhost(Pages::GhostManager *ghostManager, u8 param_2)
        {
            GhostManager *manager = GhostManager::GetStaticInstance();
            if (ghostManager->state == SAVED_GHOST_RACE_FROM_MENU)
                ghostManager->state = STAFF_GHOST_RACE_FROM_MENU;
            CosmosAssert(manager->mainGhostIndex != 0xFF, "Ghost index is null");
            manager->LoadGhost(ghostManager->rkgPointer, manager->GetGhostData(manager->mainGhostIndex)->padding);
        }
        kmCall(0x805e158c, LoadCorrectGhost);
        kmCall(0x805e15a0, LoadCorrectGhost);

        void ExtendGhostReplay(Pages::GhostManager *ghostManager, bool isStaff)
        {
            ghostManager->SetupGhostReplay(true);
            GhostManager *manager = GhostManager::GetStaticInstance();
            manager->LoadGhostReplay(ghostManager->rkgPointer, false);
        }
        kmCall(0x805e144c, ExtendGhostReplay);
        kmCall(0x805e1518, ExtendGhostReplay);

        void PatchOnWatchPress(Pages::GhostSelect *select, PushButton *button, u32 slotId)
        {
            select->OnWatchReplayPress(button, slotId);
            GhostListEntry &entry = select->ghostList->entries[select->page];
            GhostManager::GetStaticInstance()->EnableGhost(entry);
        }
        kmWritePointer(0x808bec04, PatchOnWatchPress);

        void PatchOnRunPress(Pages::GhostSelect *select, PushButton *button, u32 slotId)
        {
            select->OnChallengeGhostPress(button, slotId);
            GhostListEntry &entry = select->ghostList->entries[select->page];
            GhostManager::GetStaticInstance()->EnableGhost(entry);
        }
        kmWritePointer(0x808bebf8, PatchOnRunPress);

        void PatchBeforeInAnim(Pages::TTSplits *splitsPage)
        {
            MenuData *menuData = MenuData::GetStaticInstance();
            GlobalContext *menu98 = menuData->GetCurrentContext();

            int lap = 0x0;

            menu98->isNewTime = false;
            menu98->isNewBest = false;
            menu98->fastestLapId = -1;

            TimeEntry entry;
            RaceinfoPlayer *iPlayer = RaceInfo::GetStaticInstance()->players[0];
            RacedataPlayer *dPlayer = &RaceData::GetStaticInstance()->racesScenario.GetPlayer(0);

            splitsPage->timers[0] = *iPlayer->raceFinishTime;
            splitsPage->ctrlRaceTimeArray[0]->SetTimer(&splitsPage->timers[0]);
            splitsPage->ctrlRaceTimeArray[0]->OnFocus();

            Timer *fLap = &splitsPage->timers[0];

            u32 flapIndex = 1;

            for (int i = 1; i < splitsPage->splitsRowCount; i++)
            {
                RaceInfo::GetStaticInstance()->players[0]->FillTimerWithSplits(i, &splitsPage->timers[i]);
                if ((*fLap) > splitsPage->timers[i])
                {
                    fLap = &splitsPage->timers[i];
                    flapIndex = 1;
                }
                splitsPage->ctrlRaceTimeArray[i]->SetTimer(&splitsPage->timers[i]);
                splitsPage->ctrlRaceTimeArray[i]->OnFocus();
            }

            entry.character = dPlayer->characterId;
            entry.kart = dPlayer->kartId;
            entry.controllerType = menuData->pad.GetType(menuData->pad.GetCurrentSlotAndType(0));
            Mii *mii = menu98->playerMiis.GetMii(0);
            mii->ComputeRawMii(&entry.mii, &mii->texMap);

            if (menuData->curScene->menuId >= WATCH_GHOST_FROM_CHANNEL && menuData->curScene->menuId <= WATCH_GHOST_FROM_MENU)
            {
            }
            else
            {
                GhostManager *manager = GhostManager::GetStaticInstance();
                bool save = false;
                manager->GetLeaderboard().GhostTimeEntryToTimer(manager->entry.timer, ENTRY_FLAP);
                if (manager->entry.timer > (*fLap))
                {
                    entry.timer = *fLap;
                    save = true;
                    splitsPage->ctrlRaceTimeArray[flapIndex]->EnableFlashingAnimation();
                    menu98->fastestLapId = flapIndex;
                    manager->GetLeaderboard().Update(ENTRY_FLAP, entry, -1);
                }
                entry.timer = splitsPage->timers[0];
                s32 leaderboardPosition = -1;

#ifdef COSMOS_ANTI_CHEAT
                if (AntiCheat::GetStaticInstance()->IsRunValid() || Cosmos::Data::SettingsHolder::GetInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_GHOST_SAVING) == Cosmos::Data::DISABLED)
#endif
                    leaderboardPosition = manager->GetLeaderboard().GetLeaderboardPosition(splitsPage->timers[0]);

                menu98->leaderboardPosition = leaderboardPosition;
                splitsPage->ctrlRaceCount.isHidden = true;
                if (leaderboardPosition > -1)
                {
                    save = true;
                    if (leaderboardPosition == 0)
                    {
                        splitsPage->PlaySound(0xDD, -1);
                        splitsPage->ctrlRaceCount.isHidden = false;
                        splitsPage->ctrlRaceCount.Animate();
                        splitsPage->ctrlRaceTimeArray[0]->EnableFlashingAnimation();
                        splitsPage->savedGhostMessage.SetMsgId(0x45b, NULL);
                        menu98->isNewBest = true;
                        if (InputData::GetStaticInstance()->GetController(0).ghostWriter->state != 3)
                        {
                            if (splitsPage->timers[0].minutes < 6)
                                menu98->isNewTime = true;
                        }
                    }
                }
#ifdef COSMOS_ANTI_CHEAT
                if (!AntiCheat::GetStaticInstance()->IsRunValid())
                    save = false;
#endif
                if(manager->wereGhostsDisabled)
                    save = false;

                if (save)
                {
                    GhostData data;
                    RKG buffer;
                    data.Fill(0);
                    buffer.ClearBuffer();

                    RKG *rkg = &manager->rkg;

                    if (data.CreateRKG(&buffer) && buffer.CompressTo(rkg))
                    {
                        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
                        if (leaderboardPosition > -1)
                            manager->GetLeaderboard().Update(leaderboardPosition, entry, trackId);
                        CosmosFile::FileManager::GetStaticInstance()->taskThread->Request(&GhostManager::CreateAndSaveFiles, manager, NULL);
                    }
                }
            }
        }
        kmWritePointer(0x808DA614, PatchBeforeInAnim);
    }
}