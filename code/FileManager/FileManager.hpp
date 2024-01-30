#ifndef _COSMOS_FILE_MANAGER_
#define _COSMOS_FILE_MANAGER_

#include <FileManager/Riivolution.hpp>
#include <kamek.hpp> 
#include <core/egg/Heap.hpp>
#include <core/egg/Thread.hpp>
#include <core/System/RKSystem.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/devfs/isfs.hpp>
#include <Storage/StorageDevice.hpp>

typedef char FileName[255];
#define MAXFATFILEPATH 260

namespace CosmosFile
{
    enum FILE_MODE{
        FILE_MODE_NONE,
        FILE_MODE_READ,
        FILE_MODE_WRITE,
        FILE_MODE_READ_WRITE = FILE_MODE_READ | FILE_MODE_WRITE,
    };

    class FileManager{
        public:
            static FileManager * sInstance;
            static void CreateStaticInstance();
            static FileManager * Create();
            static FileManager * GetStaticInstance();
            
            FileManager() : fd(-1) {memset(&this->path, 0, IPCMAXPATH);}
            virtual s32 Open(const char * filepath, u32 mode);
            virtual s32 CreateOpen(const char * filepath, u32 mode);
            virtual s32 CreateFolder(const char * filepath);
            virtual s32 GetDeviceFd() const;
            virtual void GetCorrectPath(char * realPath, const char * path) const;
            virtual s32 Read(void * buffer = NULL, s32 size = 0);
            virtual s32 Write(u32 size, void * buffer);
            virtual s32 Overwrite(u32 size, void * buffer);
            virtual void Close();
            int GetFileSize() const{return this->fileSize;}
            EGG::TaskThread * taskThread;
            static const char * modPath;
            IOS::IPCPath * folderFileNames;

        protected:
            s32 fd;
            s32 fileSize;
            char path[IPCMAXPATH];
            bool isValid;
    };

    bool isRiivo();

    class RiivoFileManager : public FileManager{
        public:
            RiivoFileManager(){};
            s32 Open(const char * filepath, u32 mode) override;
            s32 CreateOpen(const char * filepath, u32 mode) override;
            s32 GetDeviceFd() const override;
            s32 CreateFolder(const char * filepath) override;
        private:
            void GetCorrectPath(char *realPath, const char *path) const override;
            RiivoMode GetRiivoMode(u32 mode) const;
    };
    
    class FatFileManager : public FileManager{
        public:
            FatFileManager(){};
            s32 Open(const char * filepath, u32 mode) override;
            s32 CreateOpen(const char * filepath, u32 mode) override;
            s32 CreateFolder(const char * filepath) override;
            s32 Read(void * buffer = NULL, s32 size = 0) override;
            s32 Write(u32 size, void * buffer) override;
            s32 Overwrite(u32 size, void * buffer) override;
            void Close() override;
        private:
            void GetPath(const char *path);
            wchar_t realPath[MAXFATFILEPATH];
            FIL currentFile;
    };

}

#endif