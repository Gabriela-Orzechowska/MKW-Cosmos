#ifndef _COSMOS_GHOST_MANAGER_
#define _COSMOS_GHOST_MANAGER_

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

namespace CosmosGhost
{

    #define GAMEMODES 2

    
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
            GhostLeaderboardFile();
            u32 signature;
            u32 version;
            u32 trackId;
            u8 ghostStatus[GAMEMODES];
            u32 reserved[4 * 0x8];
            GhostTimeEntry entry[GAMEMODES][6]; //5 times + flap
    };

        class GhostLeaderboardManager 
        {
            public:
                GhostLeaderboardManager();
                GhostLeaderboardManager(const char * folderPath, u32 id);
                //~GhostLeaderboardManager();
                s32 GetLeaderboardPosition(Timer * timer) const;
                void Update(s32 position, TimeEntry * entry, u32 id);
                void Save();
                void Save(const char * folderPath);

                void GhostTimeEntryToTimer(Timer &timer, u32 index) const;
                void GhostTimeEntryToTimeEntry(TimeEntry &entry, u32 index);

                static void CreateFile(void * id);

            private:
                GhostLeaderboardFile file __attribute__ ((aligned( 0x20 )));
                char folderPath[IPCMAXPATH];
        };


        class GhostManager{
        public:
            GhostManager();
            ~GhostManager();
            static GhostManager * GetStaticInstance() { return sInstance; }
            static GhostManager * CreateStaticInstance();
            static void DestroyStaticInstance();
            void Reset();
            void Init(u32 courseId);
            bool EnableGhost(GhostListEntry * entry);
            GhostLeaderboardManager * GetLeaderboard() {return &this->leaderboard; }
            const GhostData * GetGhostData(u32 idx) const {
                return &this->files[idx];
            }
            bool LoadGhost(RKG * rkg, u32 index);
            void LoadGhostReplay(RKG * rkg, bool isGhostRace);
            static void CreateAndSaveFiles(void * holder);

            RKG rkg  __attribute__ ((aligned( 0x20 )));
            GhostLeaderboardManager leaderboard __attribute__ ((aligned( 0x20 )));
            u32 mainGhostIndex;
            static char folderPath[IPCMAXPATH];
            CosmosFile::FolderManager * folderManager;
            TimeEntry entry;

        private:
            static GhostManager * sInstance;
            GhostData * files;
            u32 courseId;
            u32 rkgCount;
            
    };
}



#endif