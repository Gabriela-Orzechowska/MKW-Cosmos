#include <SlotExpansion/SlotExpansion.hpp>

int GetCorrectTrackBMG(int slot)
{
    if(slot < 0x100) return GetTrackBMGId((CourseId) slot);
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
    if(bmgId != 0x1101 && bmgId < 0x2498)  bmgId = GetCorrectTrackBMG(bmgId);
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
    if(slot < CT_OFFSET) snprintf(out, size, format, fileName);
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

    manager->winningCourse = manager->currentLayoutArray[manager->lastSelectedCup * 4 + scenario->settings.raceNumber]; 
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

void VSRaceOrderFix(GlobalContext& context) {

}
