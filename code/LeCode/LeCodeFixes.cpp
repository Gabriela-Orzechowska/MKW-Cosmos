#include <kamek.hpp>
#include <game/System/Archive.hpp>

void CheckArchiveBeforeClearing(ArchiveFile * file)
{
    if(file->archive != NULL && file->archiveHeap == NULL) return;
    if(file->decompressedArchive != NULL && file->decompressedArchiveHeap == NULL) return;

    file->Clear();
}

kmCall(0x8052ac70, CheckArchiveBeforeClearing);

kmWrite32(0x806394f0, 0x60000000);