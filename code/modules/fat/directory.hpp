#pragma once

#include <kamek.hpp>
#include <modules/fat/fat_types.hpp>
#include <modules/fat/partition.hpp>

#define MAX_LFN_LENGTH	256
#define MAX_ALIAS_LENGTH 13
#define LFN_ENTRY_LENGTH 13
#define ALIAS_ENTRY_LENGTH 11
#define MAX_ALIAS_EXT_LENGTH 3
#define MAX_ALIAS_PRI_LENGTH 8
#define MAX_NUMERIC_TAIL 999999
#define FAT16_ROOT_DIR_CLUSTER 0

#define DIR_SEPARATOR '/'

// File attributes
#define ATTRIB_ARCH	0x20			// Archive
#define ATTRIB_DIR	0x10			// Directory
#define ATTRIB_LFN	0x0F			// Long file name
#define ATTRIB_VOL	0x08			// Volume
#define ATTRIB_SYS	0x04			// System
#define ATTRIB_HID	0x02			// Hidden
#define ATTRIB_RO	0x01			// Read only

#define CASE_LOWER_EXT  0x10		// WinNT lowercase extension
#define CASE_LOWER_BASE 0x08		// WinNT lowercase basename



#define DIR_ENTRY_DATA_SIZE 0x20
#define MAX_FILENAME_LENGTH 768		// 256 UCS-2 characters encoded into UTF-8 can use up to 768 UTF-8 chars

namespace FAT
{
    class Partition;
    class Directory;

    class Directory {
        public:
        u8            entryData[DIR_ENTRY_DATA_SIZE];
        DIR_ENTRY_POSITION dataStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
        DIR_ENTRY_POSITION dataEnd;			// Always points to the file/directory's alias entry
        char               filename[MAX_FILENAME_LENGTH];

        bool IsDirectory()
        {
            return ((this->entryData[DIR_ENTRY_attributes] & ATTRIB_DIR) != 0);
        }
        bool IsWritable()
        {
            return ((this->entryData[DIR_ENTRY_attributes] & ATTRIB_RO) == 0);
        }
        bool IsDot()
        {
            return ((this->filename[0] == '.') && ((this->filename[1] == '\0') ||
		    ((this->filename[1] == '.') && this->filename[2] == '\0')));
        }
    };

    typedef struct DIR_STATE_STRUCT {
        Partition * partition;
        Directory  currentEntry;
        u32   startCluster;
        bool       inUse;
        bool       validEntry;
    } DIR_STATE_STRUCT;
} // namespace FAT
