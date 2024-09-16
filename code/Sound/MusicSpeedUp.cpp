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

#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/Sound/RaceAudioManager.hpp>
#include <game/Sound/Actors/KartSound.hpp>
#include <game/Sound/RSARSounds.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceGhostDiffTime.hpp>
#include <Settings/UserData.hpp>

void FinalLapSpeedUp(RaceRSARSoundsPlayer& soundPlayer, u32 lapSoundId, u32 playerId)
{
    static u8 hudIdFinalLap;

    RaceAudioMgr *audioMgr = RaceAudioMgr::sInstance;

    u8 maxLap = audioMgr->maxLap;
    u8 currentLap = audioMgr->lap;
    u8 firstPlayerId = hudIdFinalLap;

    if(maxLap == 1) return; //We dont want to do that when track is 1 lap

    if (maxLap == RaceData::GetStaticInstance()->racesScenario.GetSettings().lapCount)
    {
        using namespace Cosmos::Data;
        if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_MUSIC_CUTOFF) != CUTOFF_DISABLED)
        {
            RaceInfo * raceInfo = RaceInfo::GetStaticInstance();
            Timer * raceTimer = &raceInfo->timerManager->timers[0];
            Timer * playerTimer = &raceInfo->players[firstPlayerId]->lapSplits[maxLap-2];
            Timer difference;
            CtrlRaceGhostDiffTime::SubtractTimers(difference, raceTimer, playerTimer);

            if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_MUSIC_CUTOFF) == SPEEDUP)
            {
                if(difference.minutes < 1 && difference.seconds < 5)
                {
                    KartHolder::GetStaticInstance()->GetKart(hudIdFinalLap)->pointers.kartSound->soundArchivePlayer->soundPlayerArray->soundList.GetFront().ambientParam.pitch += 0.0002f;
                }
            }

            if(maxLap != currentLap) 
            {
                soundPlayer.PlaySound(0x74, playerId);
            }
        }
        else if ((maxLap != currentLap) && (audioMgr->raceState == 0x4 || audioMgr->raceState == 0x6)) 
        {
            audioMgr->ChangeMusic(RACE_STATE_FAST);
        }
    }
    else if(currentLap != maxLap)
    {
        hudIdFinalLap = audioMgr->playerIdFirstLocalPlayer;
        soundPlayer.PlaySound(lapSoundId, playerId);
    }
}

kmCall(0x8070b2f8, FinalLapSpeedUp);
//Patch Branches
kmWrite32(0x8070b2c0, 0x60000000); //Patch Branches
kmWrite32(0x8070b2d4, 0x60000000);
