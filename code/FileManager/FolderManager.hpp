#pragma once

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>

namespace DXFile
{
    class FolderManager
    {
        public:
            FolderManager() : fileCount(0), filenames(NULL) {}
            ~FolderManager();
            static FolderManager * Create();
            virtual void GetFilePath(char * filepath, u32 index) const;
            virtual s32 ReadFolder(const char * filepath);
            s32 ReadFile(void * buffer, u32 index, u32 mode);
            void CloseFile(){this->curFile->Close();}
            const char * GetName() const {return this->folderName;}
            const u32 GetFileCount() const {return this->fileCount; }
            void CloseFolder();
            u32 fileCount;

        protected:
            FileManager * curFile;
            char folderName[IPCMAXPATH];
            IOS::IPCPath * filenames;
            
    };
} // namespace DXFile
