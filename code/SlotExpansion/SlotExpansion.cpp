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

#include "Race/RaceData.hpp"
#include "SlotExpansion/CupManager.hpp"
#include "System/Identifiers.hpp"
#include "UI/BMG/BMG.hpp"
#include "UI/MenuData/MenuData.hpp"
#include <SlotExpansion/SlotExpansion.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>

int GetCorrectTrackBMG(int slot)
{
    if(slot == 0x1101) return slot;
    if(slot < 0x100) return GetTrackBMGId((CourseId) slot);
    if(slot >= 0x3000) return slot + BMG_GROUPS;
    return slot + BMG_TRACKS;
}

kmCall(0x807e5244, GetCorrectTrackBMG);
kmCall(0x807e608c, GetCorrectTrackBMG);
kmCall(0x807e6188, GetCorrectTrackBMG);
kmCall(0x808552bc, GetCorrectTrackBMG);
kmCall(0x8085fa08, GetCorrectTrackBMG);

kmWrite32(0x808406e8, 0x388000ff); 
kmWrite32(0x808415ac, 0x388000ff);
kmWrite32(0x80643004, 0x3be000ff);
kmWrite32(0x808394e8, 0x388000ff);
kmWrite32(0x80644104, 0x3b5b0000);

void PatchVoteBMG(VoteControl& control, bool isValid, u32 bmgId, MiiGroup* group, u32 id, bool isLocal, u32 team){
    bmgId = GetCorrectTrackBMG(bmgId);
    control.Fill(isValid, bmgId, group, id, isLocal, team);
}
kmCall(0x806441b8, PatchVoteBMG);

void PatchWinningVoteBMG(u32 trackId){
    register Pages::Vote* page;
    asm{
        ASM( mr page, r27; )
    }
    page->trackBmgId = GetCorrectTrackBMG(trackId);
}
kmCall(0x80644348, PatchWinningVoteBMG);
kmWrite32(0x8064434c, 0x48000020);

extern char * COURSE_NAMES[42]; //COURSE_NAMES

void LoadCorrectTrack(char * out, u32 size, char * format, char * fileName)
{
    int slot = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
    if(Cosmos::isRTSlot(slot)) snprintf(out, size, format, fileName);
    else {
        snprintf(out, size, "Race/Course/%03X", slot);
    }
}

kmCall(0x80540820, LoadCorrectTrack);
kmCall(0x80540870, LoadCorrectTrack);
kmCall(0x80541bc4, LoadCorrectTrack);

int SetCorrectMusic()
{
    return Cosmos::CupManager::GetStaticInstance()->GetCurrentMusicSlot();
}

kmCall(0x80711fd8, SetCorrectMusic);
kmCall(0x8071206c, SetCorrectMusic);

// Fix GP, as it uses raceScenario for getting current track

RacedataScenario * GPCorrectNextTrack(RacedataScenario * scenario)
{
    Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();

    u32 oldWinning = manager->winningCourse;
    manager->winningCourse = manager->currentLayoutArray[manager->lastSelectedCup * 4 + scenario->settings.raceNumber]; 
    if(Cosmos::isGroupSlot(manager->winningCourse)) {
        Cosmos::VariantDef* def = manager->GetVariantStruct(manager->winningCourse);
        manager->winningCourse = manager->GetRandomVariantTrack(manager->winningCourse);
        for(int i = 0; i < def->count; i++){
            if(oldWinning = def->slot[i]) {
                manager->winningCourse = oldWinning;
                break;
            }
        }
    }
    scenario->settings.courseId = (CourseId) manager->GetCurrentTrackSlot();
    return scenario;
}

asm int GPCorrectNextTrackWrapper()
{
    ASM(
        nofralloc;
        mflr r0;
        stw r0, 0x8 (r1);
        bl GPCorrectNextTrack;
        lwz r0, 0x8 (r1);
        mtlr r0;
        blr;
    )
}

kmWrite32(0x8052f224, 0x60000000);
kmBranch(0x8052f220, GPCorrectNextTrackWrapper);
kmPatchExitPoint(GPCorrectNextTrackWrapper, 0x8052f228);
kmWrite32(0x80531f80, 0x4e800020); // dont preload course due to memory

void DemoFix(register RaceData* data){
    register u32 id;
    asm {
        ASM (
        mr id, r0;
        stw r0, 0x1758(data);
            );
    };
    Cosmos::CupManager::GetStaticInstance()->winningCourse = id;
}
kmCall(0x8085a95c, DemoFix);
kmCall(0x8085a940, DemoFix);

kmWrite32(0x805bcf9c, 0x38000000);

void VSRaceOrderFix(GlobalContext& context) {
    context.vsRaceLimit = 32;
    
    register Pages::CourseSelect* course;
    asm{ASM(mr course, r29;)}
    u32 buttonId = 0;
    for(int i = 0; i < 4; i++){
        if(course->ctrlMenuCourseSelectCourse.courseButtons[i].IsSelected()) {
            buttonId = i; break;
        }
    }
    Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance(); 
    u32 cupId = manager->lastSelectedCup;
    u32 trackIndex = cupId * 4 + buttonId;
    u32 trackCount = manager->GetTrackCount();
    for(int i = 0; i < 32; i++){
        context.vsTracks[i] = (CourseId) manager->currentLayoutArray[(trackIndex + trackCount) % trackCount];
        if(Cosmos::isGroupSlot(context.vsTracks[i])) context.vsTracks[i] = (CourseId) manager->GetRandomVariantTrack(context.vsTracks[i]);
        trackIndex++;
    }
}
kmCall(0x80840a24, VSRaceOrderFix);

u32 VSRaceOrderNextTrack(u32 track){
    Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
    manager->winningCourse = track;
    return manager->GetCurrentTrackSlot();
}
kmBranch(0x808606cc, VSRaceOrderNextTrack);

void VSRaceRandomFix(GlobalContext& context){
    context.vsRaceLimit = 32;

    Random rand;
    Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
    u32 slotArray[32];
    for(int i = 0; i < 32; i++){
        bool repeat = false;
        do {
            repeat = false;
            u32 slot = rand.NextLimited(manager->GetTrackCount());
            slotArray[i] = slot;
            for(int j = 0; j < i; j++){
                if(slot == slotArray[j]) repeat = true;
            }
        } while(repeat);
    }
    for(int i = 0; i < 32; i++){
        context.vsTracks[i] = (CourseId) manager->GetTrackAtIndex(slotArray[i]);
    }
    manager->SetWinningTrack(context.vsTracks[0]);
    RaceData::GetStaticInstance()->menusScenario.GetSettings().courseId = (CourseId) manager->GetCurrentTrackSlot();
}
kmBranch(0x805e32ec, VSRaceRandomFix);
kmWrite32(0x8084e5e4, 0x60000000);


