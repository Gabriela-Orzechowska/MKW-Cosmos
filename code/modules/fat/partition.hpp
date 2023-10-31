#pragma once

#include <modules/fat/fat.hpp>

#define MIN_SECTOR_SIZE     512
#define MAX_SECTOR_SIZE     4096

namespace FAT
{
    namespace Partition
    {
        /*
        Create the fs info sector.
        */
        void CreateFSinfo(PARTITION * partition);

        /*
        Read the fs info sector data.
        */
        void ReadFSinfo(PARTITION * partition);

        /*
        Write the fs info sector data.
        */
        void WriteFSinfo(PARTITION * partition);
    }
}

