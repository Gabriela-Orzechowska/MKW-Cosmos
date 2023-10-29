#include <FileManager/FolderManager.hpp>
#include <main.hpp>

namespace DXFile
{

    FolderManager::~FolderManager()
    {
        this->CloseFolder();
        delete(this->curFile);
    }

    FolderManager * FolderManager::Create()
    {
        s32 riivo_fd = DX::Open("file", IOS::MODE_NONE);
        EGG::Heap *heap = RKSystem::mInstance.EGGSystem;
        FolderManager * manager;
        if(riivo_fd < 0){
            manager = new (heap) FolderManager();
            manager->curFile = new (heap) FileManager();
        } 
        else{ 
            IOS::Close(riivo_fd);
            manager = new (heap) FolderManager();
            manager->curFile = new (heap) RiivoFileManager();
        }     
        return manager;
    }

    s32 FolderManager::ReadFolder(const char * path)
    {
        char realPath[IPCMAXPATH];
        this->curFile->GetCorrectPath(realPath, path);  

        u32 count = MAXFILECOUNT;
        char *tmpArray = new(RKSystem::mInstance.EGGSystem, 0x20) char[255 * (count +1)];
        void *originalPtr = tmpArray;
        s32 error = ISFS::ReadDir(realPath, tmpArray, &count);
        if(error >= 0){
            strncpy(this->folderName, path, IPCMAXPATH);
            IOS::IPCPath *namesArray = new(RKSystem::mInstance.EGGSystem, 0x20) IOS::IPCPath[count];
            u32 realCount = 0;
            char curFile[IPCMAXPATH];
            while(tmpArray[0] != '\0'){
                u32 length = strlen(tmpArray);
                if(length > 255) break;
                if(length <= IPCMAXFILENAME){
                    snprintf(curFile, IPCMAXPATH, "%s/%s", realPath, tmpArray);
                    s32 curFilefd = ISFS::Open(curFile, ISFS::MODE_NONE);
                    if(curFilefd >= 0){
                        strcpy(namesArray[realCount], (char*) tmpArray);
                        realCount++;
                        ISFS::Close(curFilefd);
                    }
                }
                tmpArray = tmpArray + length + 1; 
            }
            this->fileCount = realCount;
            this->filenames = namesArray;
        }
        RKSystem::mInstance.EGGSystem->free(originalPtr);
        return error;
    }

    void FolderManager::GetFilePath(char * filepath, u32 index) const {
        snprintf(filepath, IPCMAXPATH, "%s/%s", &this->folderName, &this->filenames[index]);
        return;
    }

    s32 FolderManager::ReadFile(void * buffer, u32 index, u32 mode)
    {
        char path[IPCMAXPATH];
        this->GetFilePath(path, index);
        this->curFile->Open(path, mode);
        s32 ret = this->curFile->Read(buffer, this->curFile->GetFileSize());
        return ret;
    }

    void FolderManager::CloseFolder()
    {
        this->curFile->Close();
        if(this->filenames != nullptr) delete[](this->filenames);
        this->filenames = nullptr;
        this->folderName[0] = '\0';
        this->fileCount = 0;
    }
}