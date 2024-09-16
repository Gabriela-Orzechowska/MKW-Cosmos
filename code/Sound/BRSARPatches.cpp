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
#include <game/Sound/RSARSounds.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/Sound/RaceAudioManager.hpp>
#include <game/Sound/AudioManager.hpp>
#include <game/Sound/Actors/KartSound.hpp>
#include <game/Sound/RSARSounds.hpp>
#include <core/nw4r/snd/StrmSound.hpp>

void PatchBrsar(nw4r::snd::DVDSoundArchive::DVDFileStream& fileStream, u32 begin, u32 length)
{
    fileStream.size = 0x7FFFFFFF;
    fileStream.Seek(begin, length);
}
kmCall(0x80091354, PatchBrsar);

int PatchChannelSwitch(u32 * unknown, u32 channel, nw4r::snd::detail::BasicSound * sound)
{
    u32 soundId = sound->soundId;
    
    if(AudioManager::sInstance->soundArchivePlayer->soundArchive->GetSoundType(soundId) == SOUND_TYPE_STRM)
    {
        nw4r::snd::detail::StrmSound * strmSound = (nw4r::snd::detail::StrmSound *) sound;
        u32 neededChannels = strmSound->strmPlayer.channelsNeeded;
        u32 channelCount = strmSound->strmPlayer.strmInfo.channelCount;

        if (channelCount < neededChannels) return -1;
    }

    return soundId;
}

asm int InjectSwitchChannel()
{
    ASM(
        nofralloc;
        stwu r1, -0x80 (r1);
        stmw r3, 0x8 (r1);

        mflr r12;
        bl PatchChannelSwitch;
        mr r0, r3;

        mtlr r12;
        lmw r3, 0x8 (r1);
        addi r1, r1, 0x80;
        blr;
    );
}

kmCall(0x806fab78, InjectSwitchChannel);
