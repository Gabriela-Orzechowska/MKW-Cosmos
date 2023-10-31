#ifndef _LIBFAT_H
#define _LIBFAT_H

#include <modules/disc_io/disc_io.hpp>
#include <kamek.hpp>
#include <core/rvl/os/mutex.hpp>
#include <core/rvl/os/OS.hpp>

enum FILE_ATTR {
    ATTR_ARCHIVE	= 0x20,			/* Archive */
    ATTR_DIRECTORY	= 0x10,			/* Directory */
    ATTR_VOLUME		= 0x08,			/* Volume */
    ATTR_SYSTEM		= 0x04,			/* System */
    ATTR_HIDDEN		= 0x02,			/* Hidden */
    ATTR_READONLY	= 0x01			/* Read only */
};

typedef enum {FS_UNKNOWN, FS_FAT12, FS_FAT16, FS_FAT32} FS_TYPE;

typedef struct FAT {
	sec_t    fatStart;
	u32 sectorsPerFat;
	u32 lastCluster;
	u32 firstFree;
	u32 numberFreeCluster;
	u32 numberLastAllocCluster;
} FATDevice;

struct PARTITION {
	const DISC_INTERFACE* disc;
	struct CACHE*         cache;
	// Info about the partition
	FS_TYPE               filesysType;
	u64              totalSize;
	sec_t                 rootDirStart;
	u32              rootDirCluster;
	u32              numberOfSectors;
	sec_t                 dataStart;
	u32              bytesPerSector;
	u32              sectorsPerCluster;
	u32              bytesPerCluster;
	u32              fsInfoSector;
	FATDevice             fat;
	// Values that may change after construction
	u32              cwdCluster;			// Current working directory cluster
	int                   openFileCount;
	struct _FILE_STRUCT*  firstOpenFile;		// The start of a linked list of files
	OSMutex             lock;					// A lock for partition operations
	bool                  readOnly;				// If this is set, then do not try writing to the disc
	char                  label[12];			// Volume label
};

typedef struct {
	u32 cluster;
	sec_t sector;
	s32 byte;
} FILE_POSITION;

typedef struct {
	u32 cluster;
	sec_t    sector;
	s32  offset;
} DIR_ENTRY_POSITION;

typedef struct _FILE_STRUCT {
	u32             filesize;
	u32             startCluster;
	u32             currentPosition;
	FILE_POSITION        rwPosition;
	FILE_POSITION        appendPosition;
	DIR_ENTRY_POSITION   dirEntryStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
	DIR_ENTRY_POSITION   dirEntryEnd;		// Always points to the file's alias entry
	PARTITION*           partition;
	struct _FILE_STRUCT* prevOpenFile;		// The previous entry in a double-linked list of open files
	struct _FILE_STRUCT* nextOpenFile;		// The next entry in a double-linked list of open files
	bool                 read;
	bool                 write;
	bool                 append;
	bool                 inUse;
	bool                 modified;
} FILE_STRUCT;

typedef enum {FT_DIRECTORY, FT_FILE} FILE_TYPE;

#define DIR_ENTRY_DATA_SIZE 0x20
#define MAX_FILENAME_LENGTH 768		// 256 UCS-2 characters encoded into UTF-8 can use up to 768 UTF-8 chars

typedef struct {
	u8            entryData[DIR_ENTRY_DATA_SIZE];
	DIR_ENTRY_POSITION dataStart;		// Points to the start of the LFN entries of a file, or the alias for no LFN
	DIR_ENTRY_POSITION dataEnd;			// Always points to the file/directory's alias entry
	char               filename[MAX_FILENAME_LENGTH];
} DIR_ENTRY;

typedef struct DIR_STATE_STRUCT {
	PARTITION* partition;
	DIR_ENTRY  currentEntry;
	u32   startCluster;
	bool       inUse;
	bool       validEntry;
} DIR_STATE_STRUCT;

namespace FAT
{
    s32 Open(FILE_STRUCT * file, PARTITION * partitiom, const char * path, s32 flags);
    s32 Close(s32 fd);
    size_t Write(s32 fd, const char * ptr, size_t len);
    size_t Read(s32 fd, char * ptr, size_t len);
    s32 Seek(s32 fd, s32 position, s32 dir);
    s32 FTruncate(s32 fd, s32 size);
    s32 FSync(s32 fd);

    FILE_ATTR GetAttr(PARTITION * partition, const char * file);
    s32 SetAttr(PARTITION * partition, const char * file, FILE_ATTR attr);
    s32 Unlink(PARTITION * partition, const char * name);
    s32 Rename(PARTITION *partition, const char *oldName, const char *newName);
    s32 Chdir(PARTITION *partition, const char *name);
    s32 Mkdir(PARTITION *partition, const char *path);

    DIR_STATE_STRUCT * DirOpen(DIR_STATE_STRUCT *state, PARTITION *partition, const char *path);
    s32 DirReset(DIR_STATE_STRUCT * state);
    s32 DirNext(DIR_STATE_STRUCT * state, char * filename);
    s32 DirClose(DIR_STATE_STRUCT * state);
}

#endif