#ifndef _DX_GHOST_MANAGER_
#define _DX_GHOST_MANAGER_

#include <kamek.hpp>
#include <main.hpp>
#include <game/UI/Ctrl/UIControl.hpp>
#include <game/UI/Page/Other/GhostManager.hpp>
#include <FileManager/FileManager.hpp>
#include <FileManager/FolderManager.hpp>
#include <LeCode/LeCodeManager.hpp>
#include <game/UI/Page/Other/TTSplits.hpp>
#include <game/Race/RaceData.hpp>
#include <game/UI/Page/Other/GhostSelect.hpp>

namespace DXGhost
{
    class GhostManager{
        public:
            GhostManager();
            ~GhostManager();
            static GhostManager * GetStaticInstance() { return sInstance; }
            static GhostManager * CreateStaticInstance();
            static void DestroyStaticInstance();
            void Reset();
            void Init(u32 courseId);
            const GhostData * GetGhostData(u32 idx) const {return &this->files[idx];}
            bool LoadGhost(RKG * rkg, u32 index);
            //void LoadAllGhosts(RKG * rkg, u32 maxCount, bool isGhostRace);
            static void CreateAndSaveFiles(void * holder);

            RKG rkg  __attribute__ ((aligned( 0x20 )));
            u32 mainGhostIndex;
            static char folderPath[IPCMAXPATH];
            DXFile::FolderManager * folderManager;

        private:
            static GhostManager * sInstance;
            GhostData * files;
            u32 courseId;
            u32 rkgCount;
            u32 padding[20];
    };
}



#endif