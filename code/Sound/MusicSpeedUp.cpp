#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/Sound/RaceAudioManager.hpp>
#include <game/Sound/Actors/KartSound.hpp>
#include <game/Sound/RSARSounds.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceGhostDiffTime.hpp>

void FinalLapSpeedUp(RaceRSARSoundsPlayer *soundPlayer, u32 lapSoundId, u32 playerId)
{
    static u8 hudIdFinalLap;

    RaceAudioMgr *audioMgr = RaceAudioMgr::sInstance;

    u8 maxLap = audioMgr->maxLap;
    u8 currentLap = audioMgr->lap;
    u8 firstPlayerId = hudIdFinalLap;

    if(maxLap == 1) return; //We dont want to do that when track is 1 lap

    if (maxLap == RaceData::sInstance->racesScenario.settings.lapCount)
    {
        RaceInfo * raceInfo = RaceInfo::sInstance;
        Timer * raceTimer = &raceInfo->timerManager->timers[0];
        Timer * playerTimer = &raceInfo->players[firstPlayerId]->lapSplits[maxLap-2];
        Timer difference;
        CtrlRaceGhostDiffTime::SubtractTimers(difference, raceTimer, playerTimer);

        if(difference.minutes < 1 && difference.seconds < 5)
        {
            KartHolder::sInstance->GetKart(hudIdFinalLap)->pointers.kartSound->soundArchivePlayer->soundPlayerArray->soundList.GetFirst()->ambientParam.pitch += 0.0002f;
        }
        if(maxLap != currentLap) 
        {
            soundPlayer->PlaySound(0x74, playerId);
        }
    }
    else if(currentLap != maxLap)
    {
        hudIdFinalLap = audioMgr->playerIdFirstLocalPlayer;
        soundPlayer->PlaySound(lapSoundId, playerId);
    }
}

kmCall(0x8070b2f8, FinalLapSpeedUp);
//Patch Branches
kmWrite32(0x8070b2c0, 0x60000000); //Patch Branches
kmWrite32(0x8070b2d4, 0x60000000);