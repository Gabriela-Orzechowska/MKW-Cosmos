#pragma once

#include <kamek.hpp>
#include <FileManager/FileManager.hpp>

typedef char FATFs_Path[130];

namespace CosmosFile
{
    class FolderManager
    {
        public:
            FolderManager() : fileCount(0), filenames(NULL) {}
            ~FolderManager();
            static FolderManager * Create();
            virtual void GetFilePath(char * filepath, u32 index) const;
            virtual s32 ReadFolder(const char * filepath);
            virtual const char * GetName() {return this->folderName;}
            s32 ReadFile(void * buffer, u32 index, u32 mode);
            void CloseFile(){this->curFile->Close();}
            const u32 GetFileCount() const {return this->fileCount; }
            void CloseFolder();
            u32 fileCount;

        protected:
            FileManager * curFile;
            char folderName[IPCMAXPATH];
            IOS::IPCPath * filenames;
            
    };
    class RiivoFolderManager : public FolderManager
    {
        public:
            RiivoFolderManager(){}
            s32 ReadFolder(const char * filepath) override;
    };

    class FatFolderManager : public FolderManager
    {
        public:
            FatFolderManager(){}
            s32 ReadFolder(const char * filepath) override;
            const char * GetName() override {return this->folderPathFat;}
            void GetFilePath(char * filepath, u32 index) const override;
        private:
            void GetPath(const char *path);
            FATFs_Path * fatFileNames;
            wchar_t realPath[MAXFATFILEPATH];
            char folderPathFat[MAXFATFILEPATH];
    };
} // namespace CosmosFile
