/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
#define MAXFATFILEPATH 130

// Base and Riivo Managers are gotten from SIP, thanks to Melg


namespace CosmosFile
{
    enum FILE_MODE{
        FILE_MODE_NONE,
        FILE_MODE_READ,
        FILE_MODE_WRITE,
        FILE_MODE_READ_WRITE = FILE_MODE_READ | FILE_MODE_WRITE,
    };


    const char titlePath[] __attribute__((aligned(0x20))) = "/title/00010004";
    const char nandPath[] __attribute__((aligned(0x20))) = "/shared2/Aurora";

    class FileManager{
        public:
            static FileManager * sInstance;
            static FileManager* nandInstance;
            static void CreateStaticInstance();
            static FileManager * Create();
            static FileManager * GetStaticInstance() { return sInstance; }
            static FileManager* GetNANDManager() { return nandInstance; }
            
            FileManager() : fd(-1) {memset(&this->path, 0, IPCMAXPATH);}
            virtual s32 Open(const char * filepath, u32 mode);
            virtual s32 CreateOpen(const char * filepath, u32 mode);
            virtual s32 CreateFolder(const char * filepath);
            virtual s32 GetDeviceFd() const;
            virtual void GetCorrectPath(char * realPath, const char * path) const;
            virtual s32 Read(void * buffer = NULL, s32 size = 0);
            virtual s32 Write(u32 size, const void * buffer);
            virtual s32 Overwrite(u32 size, const void * buffer);
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
            RiivoFileManager(bool nandReplacement) : isNand(nandReplacement) {};
            s32 Open(const char * filepath, u32 mode) override;
            s32 CreateOpen(const char * filepath, u32 mode) override;
            s32 GetDeviceFd() const override;
            s32 CreateFolder(const char * filepath) override;
        private:
            void GetCorrectPath(char *realPath, const char *path) const override;
            RiivoMode GetRiivoMode(u32 mode) const;
            void GetFilePath(char* realPath, const char* path) const;
            bool isNand;
    };
    
    class FatFileManager : public FileManager{
        public:
            FatFileManager(){};
            s32 Open(const char * filepath, u32 mode) override;
            s32 CreateOpen(const char * filepath, u32 mode) override;
            s32 CreateFolder(const char * filepath) override;
            s32 Read(void * buffer = NULL, s32 size = 0) override;
            s32 Write(u32 size, const void * buffer) override;
            s32 Overwrite(u32 size, const void * buffer) override;
            void Close() override;
        private:
            void GetPath(const char *path);
            wchar_t realPath[MAXFATFILEPATH];
            FIL currentFile;
    };

}

#endif
