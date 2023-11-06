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
            manager = new (heap) RiivoFolderManager();
            manager->curFile = new (heap) RiivoFileManager();
        }     
        return manager;
    }

    s32 FolderManager::ReadFolder(const char * path)
    {
        char realPath[IPCMAXPATH];
        this->curFile->GetCorrectPath(realPath, path);  

        u32 count = MAXFILECOUNT;
        char *pathArray = new(RKSystem::mInstance.EGGSystem, 0x20) char[255 * (count +1)];
        void *originalPtr = pathArray;
        s32 error = ISFS::ReadDir(realPath, pathArray, &count);
        if(error >= 0){
            strncpy(this->folderName, path, IPCMAXPATH);
            IOS::IPCPath *namesArray = new(RKSystem::mInstance.EGGSystem, 0x20) IOS::IPCPath[count];
            u32 realCount = 0;
            char curFile[IPCMAXPATH];
            while(pathArray[0] != '\0'){
                u32 length = strlen(pathArray);
                if(length > 255) break;
                if(length <= IPCMAXFILENAME){
                    snprintf(curFile, IPCMAXPATH, "%s/%s", realPath, pathArray);
                    s32 curFilefd = ISFS::Open(curFile, ISFS::MODE_NONE);
                    if(curFilefd >= 0){
                        strcpy(namesArray[realCount], (char*) pathArray);
                        realCount++;
                        ISFS::Close(curFilefd);
                    }
                }
                pathArray = pathArray + length + 1; 
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

    s32 RiivoFolderManager::ReadFolder(const char * filepath)
    {
        s32 riivo_fd = this->curFile->GetDeviceFd();
        strncpy(this->folderName, filepath, IPCMAXPATH);

        s32 riivo_folder_fd = IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_OPENDIR, (void*)filepath, strlen(filepath)+1, NULL, 0);

        char fileName[RIIVOMAXPATH] __attribute__ ((aligned( 0x20 )));
        RiivoStats stats __attribute__ ((aligned( 0x20 )));
        IOS::IOCtlvRequest requests[3] __attribute__ ((aligned( 0x20 )));

        IOS::IPCPath *pathArray = new(RKSystem::mInstance.EGGSystem, 0x20) IOS::IPCPath[MAXFILECOUNT];
        
        u32 count = 0;
        for(; count < MAXFILECOUNT; count++)
        {
            requests[0].address = &riivo_folder_fd;
            requests[0].size = 4;
            requests[1].address = &fileName;
            requests[1].size = RIIVOMAXPATH;
            requests[2].address = &stats;
            requests[2].size = sizeof(stats);

            s32 ret = IOS::IOCtlv(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_NEXTDIR, 1, 2, requests);
            if(ret != 0) break;
            if((stats.Mode & S_IFDIR) == S_IFDIR) continue;

            strncpy(pathArray[count], fileName, IPCMAXPATH);
        }

        IOS::IPCPath * finalArray = new (RKSystem::mInstance.EGGSystem) IOS::IPCPath[count];
        memcpy(finalArray, pathArray, sizeof(IOS::IPCPath) * count);
        this->fileCount = count;
        this->filenames = finalArray;
        delete[](pathArray);

        s32 ret = IOS::IOCtl(riivo_fd, (IOS::IOCtlType)RIIVO_IOCTL_CLOSEDIR, (void *)&riivo_folder_fd, 4, NULL ,0);
        IOS::Close(riivo_fd);
        return ret;
    }
}