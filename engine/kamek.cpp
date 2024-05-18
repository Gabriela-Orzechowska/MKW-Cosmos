#include <kamek.hpp>

BootHook *BootHook::sHooks = NULL;
MenuLoadHook *MenuLoadHook::sHooks = NULL;
RaceLoadHook *RaceLoadHook::sHooks = NULL;
RacePostLoadHook *RacePostLoadHook::sHooks = NULL;
RaceFrameHook *RaceFrameHook::sHooks = NULL;
LeCodeLoadHook *LeCodeLoadHook::sHooks = NULL;
StrapEndHook *StrapEndHook::sHooks = NULL;

ReplaceFunction * ReplaceFunction::sHooks = nullptr;

kmBranch(0x800074d4, BootHook::exec);
kmBranch(0x805320d0, RaceLoadHook::exec);
kmBranch(0x8053369c, RaceFrameHook::exec); //RaceInfo::Update()
kmBranch(0x8063507c, MenuLoadHook::exec);
kmWritePointer(0x802585c8, StrapEndHook::exec);

