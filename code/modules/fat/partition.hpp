#ifndef _FAT_PARTITION_
#define _FAT_PARTITION_

#include <kamek.hpp>
#include <modules/disc_io/disc_io.hpp>

#define MIN_SECTOR_SIZE     512
#define MAX_SECTOR_SIZE     4096

namespace FAT
{
    class Partition
    {
        const DISC_INTERFACE * disc;

    };

} // namespace FAT

#endif