#include <kamek.hpp>
#include <game/System/Archive.hpp>

void CheckArchiveBeforeClearing(ArchiveFile * file)
{
    if(file->archiveHeap == NULL) return;
    if(file->decompressedArchiveHeap == NULL) return

    file->Clear();
}

kmCall(0x8052ac70, CheckArchiveBeforeClearing);