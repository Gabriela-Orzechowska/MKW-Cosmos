#include <Ghost/GhostManager.hpp>

void CorrectGhostTrackName(LayoutUIControl * control, const char *textBoxName, u32 bmgId, const TextInfo *text)
{
    u32 trackId = LeCode::LeCodeManager::GetStaticInstance()->GetTrackID();
    bmgId = trackId + 0x7000;
    control->SetTextBoxMsg(textBoxName, bmgId, text);
    return;
}

kmCall(0x805e2a4c, CorrectGhostTrackName);

namespace DXGhost
{
    GhostManager * GhostManager::sInstance = NULL;
    char GhostManager::folderPath[IPCMAXPATH] = "";

    GhostManager::GhostManager() : courseId(-1)
    {
        this->folderManager = DXFile::FolderManager::Create();
        this->files = NULL;
    }

    GhostManager::~GhostManager() 
    {
        delete(this->folderManager);
    }

    GhostManager * GhostManager::CreateStaticInstance()
    {
        GhostManager * holder = GhostManager::sInstance;
        if(holder == nullptr)
        {
            holder = new(RKSystem::mInstance.EGGSystem) GhostManager;
            GhostManager::sInstance = holder;
        }
        holder->Reset();
        return holder;
    }

    void GhostManager::DestroyStaticInstance()
    {
        if(GhostManager::sInstance != nullptr) delete(GhostManager::sInstance);
        GhostManager::sInstance = nullptr;
    }

    const char * ttFolders[2] = { "150", "200", };

    void GhostManager::Init(u32 courseId)
    {
        this->Reset();
        this->courseId = courseId;
        DXFile::FolderManager * manager = this->folderManager;

        snprintf(folderPath, IPCMAXPATH, "%s/%03x", DX::ghostFolder, courseId);
        DXFile::FileManager::sInstance->CreateFolder(folderPath);

        char folderModePath[IPCMAXPATH];
        snprintf(folderModePath, IPCMAXPATH, "%s/%s", folderPath, ttFolders[DX::TTMode]);
        DXFile::FileManager::sInstance->CreateFolder(folderModePath);

        manager->ReadFolder(folderModePath);

        this->files = new (RKSystem::mInstance.EGGSystem) GhostData[manager->GetFileCount()];
        RKG * rkg = &this->rkg;
        u32 counter = 0;
        for(int i = 0; i < manager->GetFileCount(); i++)
        {
            rkg->ClearBuffer();
            GhostData * header = &this->files[counter];
            header->isValid = false;
            if(manager->ReadFile(rkg, i, DXFile::FILE_MODE_READ) > 0 && rkg->CheckValidity())
            {
                rkg->header.ghostType = (GhostType) 0x7;
                header->Init(rkg);
                counter++;
            }
            manager->CloseFile();
        }
        this->rkgCount = counter;

    }

    void GhostManager::Reset()
    {
        this->courseId = -1;
        mainGhostIndex = 0xFF;
        delete[] this->files;
        this->files = nullptr;
        RaceData * raceData = RaceData::sInstance;
        raceData->menusScenario.players[1].playerType = PLAYER_NONE;
    }

    bool GhostManager::LoadGhost(RKG * rkg, u32 index)
    {
        rkg->ClearBuffer();
        this->folderManager->ReadFile(rkg, index, DXFile::FILE_MODE_READ);
        return rkg->CheckValidity();
    }

    void GhostManager::CreateAndSaveFiles(void * holder)
    {
        char path[IPCMAXPATH];
        GhostManager * manager = (GhostManager *) holder;
        RKG * rkg = &manager->rkg;
        snprintf(path, IPCMAXPATH, "%s/%01dm%02ds%03d.rkg", manager->folderManager->GetName(), rkg->header.minutes, rkg->header.seconds, rkg->header.milliseconds);
        DXFile::FileManager * fileManager = DXFile::FileManager::sInstance;
        fileManager->CreateOpen(path, DXFile::FILE_MODE_WRITE);
        u32 size = sizeof(RKG);
        if(rkg->header.compressed) size = ((CompressedRKG*)rkg)->dataLength + sizeof(RKGHeader) + 0x4 + 0x4;
        fileManager->Overwrite(size, rkg);
        fileManager->Close();
    }

    GhostLeaderboardManager::GhostLeaderboardManager()
    {
        memset(&this->file, 0, sizeof(GhostLeaderboardManager));
        this->file.signature = 0x44584C44;
        this->file.version = 0x10;
        for(int i = 0; i < GAMEMODES; i++) this->file.ghostStatus[i] = 0x0;
    }

    GhostLeaderboardManager::GhostLeaderboardManager(const char * folderPath, u32 id)
    {
        new (this) GhostLeaderboardManager();
        char filePath[IPCMAXPATH];
        snprintf(filePath, IPCMAXPATH, "%s/ld.glm", folderPath);
        strncpy(this->folderPath, folderPath, IPCMAXPATH);
        DXFile::FileManager * manager = DXFile::FileManager::sInstance;
        s32 ret = manager->Open(filePath, DXFile::FILE_MODE_READ);
        if(ret > 0) ret = manager->Read(&this->file, sizeof(GhostLeaderboardFile));
        if(ret <= 0)
        {
            manager->taskThread->Request(&GhostLeaderboardManager::CreateFile, (void*)id, NULL);
        }


        manager->Close();
    }

    void GhostLeaderboardManager::CreateFile(void * id)
    {
        char filePath[IPCMAXPATH];
        snprintf(filePath, IPCMAXPATH, "%s/ld.glm", GhostManager::folderPath);
        DXFile::FileManager * manager = DXFile::FileManager::sInstance;
        manager->CreateOpen(filePath, DXFile::FILE_MODE_READ_WRITE);
        GhostLeaderboardFile * file = new (RKSystem::mInstance.EGGSystem, 0x20) GhostLeaderboardFile;
        file->trackId = (u32) id;
        manager->Overwrite(sizeof(GhostLeaderboardFile), file);
        manager->Close();
        delete(file);
    }

    s32 GhostLeaderboardManager::Update(s32 position, TimeEntry * entry, u32 id)
    {
        DX::TT_MODE mode = DX::TTMode;
        GhostLeaderboardFile * lfile = &this->file;
        GhostTimeEntry * tentry = &lfile->entry[mode][position];
        if(position != ENTRY_FLAP)
        {
            for(int i = ENTRY_5TH; i > position; i--)
            {
                memcpy(&lfile->entry[mode][i], &lfile->entry[mode][i-1], sizeof(GhostTimeEntry));
            }
        }
        memcpy(&tentry->mii, &entry->mii, sizeof(RawMii));
        tentry->minutes = entry->timer.minutes;
        tentry->seconds = entry->timer.seconds;
        tentry->miliseconds = entry->timer.milliseconds;
        tentry->isActive = entry->timer.isActive;
        tentry->controllerType = entry->controllerType;
        tentry->kart = entry->kart;
        tentry->character = entry->character;
    }

    s32 GhostLeaderboardManager::Save()
    {
        GhostLeaderboardManager::Save(this->folderPath);
    }

    s32 GhostLeaderboardManager::Save(const char * folderPath)
    {
        char filePath[IPCMAXPATH];
        snprintf(filePath, IPCMAXPATH, "%s/ld.glm", folderPath);
        DXFile::FileManager * manager = DXFile::FileManager::sInstance;
        manager->Open(filePath, DXFile::FILE_MODE_WRITE);
        manager->Overwrite(sizeof(GhostLeaderboardFile), &this->file);
        manager->Close();
    }

    void GhostLeaderboardManager::GhostTimeEntryToTimer(Timer &timer, u32 index) const
    {
        DX::TT_MODE mode = DX::TTMode;
        timer.minutes = this->file.entry[mode][index].minutes;
        timer.seconds = this->file.entry[mode][index].seconds;
        timer.milliseconds = this->file.entry[mode][index].miliseconds;
        timer.isActive = this->file.entry[mode][index].isActive;
    }

    void GhostLeaderboardManager::GhostTimeEntryToTimeEntry(TimeEntry &entry, u32 index)
    {
        this->GhostTimeEntryToTimer(entry.timer, index);
        DX::TT_MODE mode = DX::TTMode;
        memcpy(&entry.mii, &this->file.entry[mode][index].mii, sizeof(RawMii));
        entry.character = this->file.entry[mode][index].character;
        entry.kart = this->file.entry[mode][index].kart;
        entry.controllerType = this->file.entry[mode][index].controllerType;
    }

    TimeEntry * GetTimeEntry(u32 index)
    {
        GhostManager * manager = GhostManager::GetStaticInstance();
        manager->GetLeaderboard()->GhostTimeEntryToTimeEntry(manager->entry, index);
        return &manager->entry;
    }

    void CustomGhostGroup(GhostList * list, u32 id)
    {
        u32 trackID = LeCode::LeCodeManager::GetStaticInstance()->GetTrackID();
        GhostManager * manager = GhostManager::GetStaticInstance();
        manager->Init(trackID);
        u32 index = 0;
        for(int i = 0; i < manager->folderManager->GetFileCount(); i++)
        {
            if(index == 38) break;
            const GhostData * data = manager->GetGhostData(i);
            if(data->isValid)
            {
                list->entries[index].data = data;
                index++;
            }
        }
        list->count = index;
        qsort(list, list->count, sizeof(GhostListEntry), (int (*)(const void *, const void *))*&GhostList::CompareEntries);
    };
    kmCall(0x806394f0, CustomGhostGroup);

    void * CreatePageAndManager()
    {
        GhostManager::CreateStaticInstance();
        return new(Pages::GhostSelect);
    }
    kmCall(0x80623dec, CreatePageAndManager);
    kmWrite32(0x80623de0, 0x60000000);

    void DestroyManager(Scene *scene, PageId pageId){
        scene->CreatePage(pageId);
        GhostManager::DestroyStaticInstance();
    }
    kmCall(0x8062cf98, DestroyManager);
}