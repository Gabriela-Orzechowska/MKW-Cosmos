#include <FileManager/FileManager.hpp>
#include <main.hpp>

namespace DXFile
{
    FileManager * FileManager::sInstance = NULL;

    void FileManager::CreateStaticInstance()
    {
        FileManager * manager;
        bool valid = false;
        EGG::Heap * heap = RKSystem::mInstance.EGGSystem;
        s32 ret = DX::Open("file", IOS::MODE_NONE); //Check if its riivo
        if(ret < 0) //If not, check if dolphin
        {
            ret = DX::Open("/dev/dolphin", IOS::MODE_NONE);
            if(ret >= 0)
            {
                valid = true;
                IOS::Close(ret);
            }
            manager = new (heap) FileManager();
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
        FileManager::sInstance->taskThread = EGG::TaskThread::Create(2, 30, 0x2000, NULL);
    }


    BootHook SetUpFileLoader(FileManager::CreateStaticInstance, FIRST);

    FileManager * FileManager::GetStaticInstance()
    {
        return FileManager::sInstance;
    }

    void FileManager::GetCorrectPath(char * realPath, const char * path) const
    {
        snprintf(realPath, IPCMAXPATH, "%s%s", "/title/00010004", path);
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

        this->fd = DX::Open(realPath, (IOS::Mode) mode);
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

    s32 FileManager::Write(u32 size, void *buffer){
        if(this->fd < 0) return -1;
        return IOS::Write(this->fd, buffer, size);
    }

    s32 FileManager::Overwrite(u32 size, void *buffer){
        if(this->fd < 0) return -1;
        IOS::Seek(this->fd, 0, IOS::SEEK_START);
        return IOS::Write(this->fd, buffer, size);
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
        return DX::Open("file", IOS::MODE_NONE);
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

}