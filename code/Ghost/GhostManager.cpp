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