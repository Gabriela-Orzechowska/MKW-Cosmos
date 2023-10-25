#ifndef _DX_FILE_MANAGER_
#define _DX_FILE_MANAGER_

#include <FileManager/Riivolution.hpp>
#include <kamek.hpp> 
#include <core/egg/Heap.hpp>
#include <core/egg/Thread.hpp>
#include <core/System/RKSystem.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/devfs/isfs.hpp>

typedef char FileName[255];

namespace DXFile
{
    enum FILE_MODE{
        FILE_MODE_NONE,
        FILE_MODE_READ,
        FILE_MODE_WRITE,
        FILE_MODE_READ_WRITE = FILE_MODE_READ | FILE_MODE_WRITE,
    };

    class FileManager{
        protected:
            s32 fd;
            s32 fileSize;
            char path[IPCMAXPATH];
            bool isValid;

        public:
            static FileManager * sInstance;
            static void CreateStaticInstance();
            static FileManager * GetStaticInstance();
            FileManager() : fd(-1) {memset(&this->path, 0, IPCMAXPATH);}
            virtual s32 Open(const char * filepath, u32 mode);
            virtual s32 CreateOpen(const char * filepath, u32 mode);
            virtual s32 CreateFolder(const char * filepath);
            virtual s32 GetDeviceFd() const;
            virtual void GetCorrectPath(char * realPath, const char * path) const;
            s32 Read(void * buffer = NULL, s32 size = 0);
            s32 Write(u32 size, void * buffer);
            s32 Overwrite(u32 size, void * buffer);
            void Close();
            int GetFileSize() const;
            EGG::TaskThread * taskThread;
            static const char * modPath;
    };

    bool isRiivo();

    class RiivoFileManager : public FileManager{
        public:
            RiivoFileManager(){};
            s32 Open(const char * filepath, u32 mode) override;
            s32 CreateOpen(const char * filepath, u32 mode) override;
            s32 GetDeviceFd() const override;
        private:
            void GetCorrectPath(char *realPath, const char *path) const override;
            RiivoMode GetRiivoMode(u32 mode) const;
    };

}

#endif