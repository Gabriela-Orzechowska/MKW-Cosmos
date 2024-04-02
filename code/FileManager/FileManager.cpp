#include <FileManager/FileManager.hpp>
#include <main.hpp>
#include <Storage/SDStorage.hpp>

namespace CosmosFile
{
    FileManager * FileManager::sInstance = NULL;

    void FileManager::CreateStaticInstance()
    {
        FileManager * manager;
        bool valid = false;
        EGG::Heap * heap = RKSystem::mInstance.EGGSystem;
        s32 ret = Cosmos::Open("file", IOS::MODE_NONE); //Check if its riivo
        if(ret < 0) //If not, check if dolphin
        {
            ret = Cosmos::Open("/dev/dolphin", IOS::MODE_NONE);
            if(ret >= 0)
            {
                IOS::Close(ret);
            }
            valid = true;
            SDStorage::Init();
            manager = new (heap) FatFileManager();
        }
        else
        {
            valid = true;
            IOS::Close(ret);
            manager = new (heap) RiivoFileManager();
        }
        FileManager::sInstance = manager;
        manager->isValid = true;
        if(!valid || manager == nullptr)
        {
            u32 black = 0x000000FF;
            u32 white = 0xFFFFFFFF;
            OSFatal(&white, &black, "Could not create FileManager");
        }
        CosmosLog("Created FileManager, creating task...\n");
        FileManager::sInstance->taskThread = EGG::TaskThread::Create(2, 30, 0x2000, NULL);
        CosmosLog("Task Created\n");
    }


    BootHook SetUpFileLoader(FileManager::CreateStaticInstance, FIRST);

    FileManager * FileManager::GetStaticInstance()
    {
        return FileManager::sInstance;
    }

    void FileManager::GetCorrectPath(char * realPath, const char * path) const
    {
        snprintf(realPath, IPCMAXPATH, "%s%s", "/title/00010004/", path);
        return;
    }

    s32 FileManager::GetDeviceFd() const{
    return IOS::fs_fd;
    }

    s32 FileManager::Open(const char * path, u32 mode)
    {
        char realPath[IPCMAXPATH];
        this->GetCorrectPath(realPath, path);
        strcpy(this->path, realPath);

        this->fd = Cosmos::Open(realPath, (IOS::Mode) mode);
        this->fileSize = IOS::Seek(this->fd, 0, IOS::SEEK_END);

        IOS::Seek(this->fd, 0, IOS::SEEK_START);
        return this->fd;
    }

    s32 FileManager::CreateOpen(const char * path, u32 mode)
    {
        char realPath[IPCMAXPATH];
        this->GetCorrectPath(realPath, path);
        ISFS::CreateFile(realPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
        return this->Open(path, mode);
    }

    s32 FileManager::CreateFolder(const char * path)
    {
        char realPath[IPCMAXPATH];
        this->GetCorrectPath(realPath, path);
        return ISFS::CreateDir(realPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
    }

    s32 FileManager::Read(void * buffer, s32 size)
    {
        if(this->fd < 0) return -1;
        return IOS::Read(this->fd, buffer, size);
    }

    s32 FileManager::Write(u32 size, const void *buffer){
        if(this->fd < 0) return -1;
        return IOS::Write(this->fd, (void *) buffer, size);
    }

    s32 FileManager::Overwrite(u32 size, const void *buffer){
        if(this->fd < 0) return -1;
        IOS::Seek(this->fd, 0, IOS::SEEK_START);
        return IOS::Write(this->fd, (void *) buffer, size);
    }

    void FileManager::Close(){
        if(this->fd >=0) IOS::Close(this->fd);
        this->fd = -1;
        this->fileSize = -1;
    }

    s32 RiivoFileManager::Open(const char * filepath, u32 mode){
        return FileManager::Open(filepath, this->GetRiivoMode(mode));
    }

    s32 RiivoFileManager::CreateOpen(const char * filepath, u32 mode){
        s32 riivo_fd = this->GetDeviceFd();

        IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_CREATEFILE, (void*) filepath, strlen(filepath)+1, NULL, 0);
        IOS::Close(riivo_fd);
        return this->Open(filepath, mode);
    }

    s32 RiivoFileManager::GetDeviceFd() const{
        return Cosmos::Open("file", IOS::MODE_NONE);
    }

    void RiivoFileManager::GetCorrectPath(char * realPath, const char * path) const
    {
        snprintf(realPath, IPCMAXPATH, "%s%s", "file", path);
    }

    RiivoMode RiivoFileManager::GetRiivoMode(u32 mode) const{
        if((mode & 0x3) != 0) mode -= 1;
        return (RiivoMode) mode;
    }

    s32 RiivoFileManager::CreateFolder(const char * filepath)
    {
        char realPath[IPCMAXPATH];
        s32 riivo_fd = this->GetDeviceFd();
        s32 error = IOS::IOCtl(riivo_fd, (IOS::IOCtlType) RIIVO_IOCTL_CREATEDIR, (void *) filepath, strlen(filepath)+1, NULL, 0);
        IOS::Close(riivo_fd);
        return error;
    }

    void FatFileManager::GetPath(const char *path)
    {
        mbstowcs(this->realPath, path, strlen(path)+1);
        return;
    }

    s32 FatFileManager::Open(const char * filepath, u32 mode)
    {
        if(StorageDevice::currentDevice == nullptr) return -1;
        u32 actualMode = 0x0;
        if(mode & FILE_MODE_READ)
            actualMode |= FA_READ;
        if(mode & FILE_MODE_WRITE)
            actualMode |= FA_WRITE;
        if(mode & FA_CREATE_ALWAYS)
            actualMode |= FA_OPEN_APPEND; // This is so bad lmao

        this->GetPath(filepath);
        FRESULT ret = f_open(&this->currentFile, this->realPath, actualMode);
        if(ret == FR_OK) {
            this->fileSize = f_size(&this->currentFile);
            f_lseek(&this->currentFile, 0);
            CosmosLog("Opened file: %s\n", filepath);
        }
        else CosmosError("Opening file failed: %s\n", filepath);
        
        if(ret == FR_OK) return 1;
        
        return -ret;
    }

    s32 FatFileManager::CreateOpen(const char * filepath, u32 mode)
    {
        return this->Open(filepath, mode | FA_CREATE_ALWAYS);
    }

    s32 FatFileManager::CreateFolder(const char * filepath)
    {
        if(StorageDevice::currentDevice == nullptr) return -1;
        this->GetPath(filepath);
        FRESULT ret = f_mkdir(this->realPath);
        return ret;
    }

    s32 FatFileManager::Read(void * buffer, s32 size)
    {
        if(StorageDevice::currentDevice == nullptr) 
        {
            CosmosLog("Current device is null?!\n");
            return -1;
        }
        UINT readSize;
        FRESULT ret = f_read(&this->currentFile, buffer, size, &readSize);
        if(ret != FR_OK) 
        {
            CosmosLog("File read failed! Error: %i\n", ret);
            return -1;
        }
        else
        {
            CosmosLog("File read: %i/%i\n", readSize, size);
        }
        return readSize;
    }

    s32 FatFileManager::Write(u32 size, const void * buffer)
    {
        if(StorageDevice::currentDevice == nullptr) return -1;
        UINT writtenSize;
        f_lseek(&this->currentFile, this->fileSize);
        FRESULT ret = f_write(&this->currentFile, buffer, size, &writtenSize);
        if(ret != FR_OK) 
        {
            CosmosLog("File save failed! Error: %i\n", ret);
            return -1;
        }
        CosmosLog("File saved: %i/%i", writtenSize, size);
        return writtenSize;
    }

    s32 FatFileManager::Overwrite(u32 size, const void * buffer)
    {
        if(StorageDevice::currentDevice == nullptr) return -1;
        UINT writtenSize;
        f_lseek(&this->currentFile, 0);
        FRESULT ret = f_write(&this->currentFile, buffer, size, &writtenSize);
        if(ret != FR_OK) 
        {
            CosmosLog("File save failed! Error: %i\n", ret);
            return -1;
        }
        else
        {
            CosmosLog("File saved: %i/%i\n", writtenSize, size);
        }
        return writtenSize;
    }

    void FatFileManager::Close()
    {
        if(StorageDevice::currentDevice == nullptr) return;
        f_close(&this->currentFile);
        CosmosLog("Closed file.\n");
        
        return;
    }

}