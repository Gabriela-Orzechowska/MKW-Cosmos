#include <kamek.hpp>
#include <game/Sound/RSARSounds.hpp>
#include <game/Sound/AudioManager.hpp>
#include <game/Sound/RaceAudioManager.hpp>

#define CUSTOM_WIN_MUSIC
#ifdef CUSTOM_WIN_MUSIC

const char * winReplaceTable[][2] = {
    { "strm/o_FanfareTA1st_32.brstm" , "/sound/strm/o_Fanfare1st_char_%s.brstm" },
    { "strm/o_FanfareTA1st_only32.brstm" , "/sound/strm/o_Fanfare1st_char_%sonly.brstm" },
    { "strm/o_FanfareGP1_32.brstm" , "/sound/strm/o_Fanfare1st_char_%s.brstm" },
    { "strm/o_FanfareGP1_only32.brstm" , "/sound/strm/o_Fanfare1st_char_%sonly.brstm" },
};

static char songBuffer[0x100];

extern char * charNames[48]; //808b3a90

char * changeWinningMusic(char * dest, char * source, size_t num)
{
    char * test = dest;
    char * saveSource = source;
    char * ret = nullptr;
    for(int i = 0; i < 4; i++)
    {
        if(strcmp(source, winReplaceTable[i][0]) == 0)
        {
            RacedataPlayer * dPlayer = &RaceData::sInstance->racesScenario.players[RaceInfo::sInstance->playerIdInEachPosition[0]];
            char * charName = charNames[(int)dPlayer->characterId];

            snprintf(songBuffer, 0x100, winReplaceTable[i][1], charName);
            ret = songBuffer;
            CosmosLog("Redirect: /sound/%s -> %s\n", source, ret);
        }
    }
    if(ret == nullptr)
    {
        ret = strncat(dest, source, num);
        return ret;
    }
    DVDFileInfo fileInfo;
    if(!DVDOpen(ret, &fileInfo))
    {
        char * tmp = ret;
        ret = strncat(dest,saveSource,num);
        CosmosLog("Redirect Failed: %s -> %s\n", tmp, ret);
    }
    else DVDClose(&fileInfo);
    
    strcpy(dest, ret);
    CosmosLog("Sound Name: %s\n", dest);
    
    return ret;
}

kmCall(0x8009e0b8, changeWinningMusic);

#endif