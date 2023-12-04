#pragma once
#include <kamek.hpp>

namespace FAT
{
    typedef enum {FS_UNKNOWN, FS_FAT12, FS_FAT16, FS_FAT32} FS_TYPE;
    typedef enum {FT_DIRECTORY, FT_FILE} FILE_TYPE; 

    typedef struct {
        u32 fatStart;
        u32 sectorsPerFat;
        u32 lastCluster;
        u32 firstFree;
        u32 numberFreeCluster;
        u32 numberLastAllocCluster;
    } FAT_t;  

    class Partition;
    class Directory;

    typedef struct {
        u32 cluster;
        u32 sector;
        s32 byte;
    } FILE_POSITION;

    struct DIR_ENTRY_POSITION {
        u32 cluster;
        u32    sector;
        s32  offset;
    };

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

    struct _FILE_STRUCT {
        u32             filesize;
        u32             startCluster;
        u32             currentPosition;
        FILE_POSITION        rwPosition;
        FILE_POSITION        appendPosition;
        DIR_ENTRY_POSITION   dirEntryStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
        DIR_ENTRY_POSITION   dirEntryEnd;		// Always points to the file's alias entry
        Partition*           partition;
        _FILE_STRUCT* prevOpenFile;		// The previous entry in a double-linked list of open files
        _FILE_STRUCT* nextOpenFile;		// The next entry in a double-linked list of open files
        bool                 read;
        bool                 write;
        bool                 append;
        bool                 inUse;
        bool                 modified;
    };
} // namespace FAT


