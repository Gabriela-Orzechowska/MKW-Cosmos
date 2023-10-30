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

    #define GAMEMODES 2

    class GhostManager{
        public:
            GhostManager();
            ~GhostManager();
            static GhostManager * GetStaticInstance() { return sInstance; }
            static GhostManager * CreateStaticInstance();
            static void DestroyStaticInstance();
            void Reset();
            void Init(u32 courseId);
            GhostLeaderboardManager * GetLeaderboard() {return &this->leaderboard; }
            const GhostData * GetGhostData(u32 idx) const {return &this->files[idx];}
            bool LoadGhost(RKG * rkg, u32 index);
            //void LoadAllGhosts(RKG * rkg, u32 maxCount, bool isGhostRace);
            static void CreateAndSaveFiles(void * holder);

            RKG rkg  __attribute__ ((aligned( 0x20 )));
            GhostLeaderboardManager leaderboard __attribute__ ((aligned( 0x20 )));
            u32 mainGhostIndex;
            static char folderPath[IPCMAXPATH];
            DXFile::FolderManager * folderManager;
            TimeEntry entry;

        private:
            static GhostManager * sInstance;
            GhostData * files;
            u32 courseId;
            u32 rkgCount;
            
    };

    class GhostLeaderboardManager 
    {
        public:
            GhostLeaderboardManager();
            GhostLeaderboardManager(const char * folderPath, u32 id);
            ~GhostLeaderboardManager();
            s32 GetLeaderboardPosition(Timer * timer) const;
            s32 Update(s32 position, TimeEntry * entry, u32 id);
            s32 Save();
            s32 Save(const char * folderPath);

            void GhostTimeEntryToTimer(Timer &timer, u32 index) const;
            void GhostTimeEntryToTimeEntry(TimeEntry &entry, u32 index);

            static void CreateFile(void * id);

        private:
            GhostLeaderboardFile file __attribute__ ((aligned( 0x20 )));
            char folderPath[IPCMAXPATH];
    };

    enum LEADERBOARDENTRY
    {
        ENTRY_1ST,
        ENTRY_2ND,
        ENTRY_3RD,
        ENTRY_4TH,
        ENTRY_5TH,
        ENTRY_FLAP,
    };

    #pragma (pack, 1)
    class GhostTimeEntry
    {
        public:
            GhostTimeEntry()
            {
                minutes =0;
                seconds =0;
                miliseconds = 0;
            }

            u16 minutes;
            u8 seconds;
            u16 miliseconds;
            CharacterId character;
            KartId kart;
            u32 controllerType;
            RawMii mii;
            bool isActive;
    };
    #pragma (pop)
    class GhostLeaderboardFile
    {
        public:
            u32 signature;
            u32 version;
            u32 trackId;
            u8 ghostStatus[GAMEMODES];
            u32 reserved[4 * 0x8];
            GhostTimeEntry entry[GAMEMODES][6]; //5 times + flap
    };
}



#endif