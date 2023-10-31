#pragma once

#include <kamek.hpp>

#include "fat.hpp"
#include "common.hpp"
#include "partition.hpp"

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

enum DIR_ENTRY_offset {
	DIR_ENTRY_name = 0x00,
	DIR_ENTRY_extension = 0x08,
	DIR_ENTRY_attributes = 0x0B,
	DIR_ENTRY_caseInfo = 0x0C,
	DIR_ENTRY_cTime_ms = 0x0D,
	DIR_ENTRY_cTime = 0x0E,
	DIR_ENTRY_cDate = 0x10,
	DIR_ENTRY_aDate = 0x12,
	DIR_ENTRY_clusterHigh = 0x14,
	DIR_ENTRY_mTime = 0x16,
	DIR_ENTRY_mDate = 0x18,
	DIR_ENTRY_cluster = 0x1A,
	DIR_ENTRY_fileSize = 0x1C
};

namespace FAT
{
    namespace Directory
    {
        static inline bool IsDirectory(DIR_ENTRY * entry)
        {
            return ((entry->entryData[DIR_ENTRY_attributes] & ATTRIB_DIR) != 0);
        }
        static inline bool IsWritable(DIR_ENTRY * entry)
        {
            return ((entry->entryData[DIR_ENTRY_attributes] & ATTRIB_RO) == 0);
        }
        static inline bool IsDot(DIR_ENTRY * entry)
        {
            return ((entry->filename[0] == '.') && ((entry->filename[1] == '\0') ||
		    ((entry->filename[1] == '.') && entry->filename[2] == '\0')));
        }
        bool GetFirstEntry(PARTITION * partition, DIR_ENTRY * entry, u32 dirCluster);
        bool GetNextEntry(PARTITION * partition, DIR_ENTRY * entry);
        bool EntryFromPath (PARTITION* partition, DIR_ENTRY* entry, const char* path, const char* pathEnd);
        bool Chdir (PARTITION* partition, const char* path);
        bool RemoveEntry (PARTITION* partition, DIR_ENTRY* entry);
        bool AddEntry (PARTITION* partition, DIR_ENTRY* entry, u32 dirCluster);
        u32 EntryGetCluster (PARTITION* partition, const u8 * entryData); 
        bool EntryFromPosition (PARTITION* partition, DIR_ENTRY* entry);
        bool GetVolumeLabel (PARTITION* partition, char *label);
    }
}