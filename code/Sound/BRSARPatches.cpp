#include <kamek.hpp>
#include <game/Sound/RSARSounds.hpp>

void PatchBrsar(nw4r::snd::DVDSoundArchive::DVDFileStream * fileStream, u32 begin, u32 length)
{
    fileStream->size = 0x7FFFFFFF;
    fileStream->Seek(begin, length);
}
kmCall(0x80091354, PatchBrsar);
