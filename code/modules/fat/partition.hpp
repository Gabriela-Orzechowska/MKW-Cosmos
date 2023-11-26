#ifndef _FAT_PARTITION_
#define _FAT_PARTITION_

#include <kamek.hpp>
#include <modules/disc_io/disc_io.hpp>
#include "lock.hpp"
#include "cache.hpp"

#define MIN_SECTOR_SIZE     512
#define MAX_SECTOR_SIZE     4096

#define CLUSTER_EOF_16	0xFFFF
#define	CLUSTER_EOF		0x0FFFFFFF
#define CLUSTER_FREE	0x00000000
#define CLUSTER_ROOT	0x00000000
#define CLUSTER_FIRST	0x00000002
#define CLUSTER_ERROR	0xFFFFFFFF

#define CLUSTERS_PER_FAT12 4085
#define CLUSTERS_PER_FAT16 65525


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

    typedef struct {
        u32 cluster;
        u32 sector;
        s32 byte;
    } FILE_POSITION;

    typedef struct {
        u32 cluster;
        u32    sector;
        s32  offset;
    } DIR_ENTRY_POSITION;

    class Partition;

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

    class Partition
    {
        public:
        ~Partition();
        const DISC_INTERFACE * disc;
        Cache * cache;
        FS_TYPE filesysType;
        u64 totalSize;
        u32 rootDirStart;
        u32 rootDirCluster;
        u32 sectorCount;
        u32 dataStart;
        u32 bytesPerSector;
        u32 sectorsPerCluster;
        u32 bytesPerCluster;
        u32 fsInfoSector;
        FAT_t fat;
        u32 cwdCluster;
        s32 openFileCount;
        _FILE_STRUCT * firstOpenFile;
        OSMutex lock;
        bool readOnly;
        char label[12];

        void Deinit();
        void CreateFSInfo();
        void ReadFSInfo();
        void WriteFSInfo();
        bool GetPartition(const DISC_INTERFACE * disc, u8 * cacheSpace, size_t cacheSize, u32 startSector);

        //FAT Libs
        u32 NextCluster(u32 cluster);
        u32 LinkFreeCluster(u32 cluster);
        u32 LinkFreeClusterCleared(u32 cluster);
        bool ClearLinks(u32 cluste);
        u32 TrimChain(u32 startCluster, u32 length);
        u32 LastCluster(u32 cluster);
        u32 FreeClusterCount();

        u32 ClusterToSector(u32 cluster){
            return (cluster >= CLUSTER_FIRST) ? ((cluster - CLUSTER_FIRST) * this->sectorsPerCluster) + this->dataStart : this->rootDirStart;  
        }
        bool IsValidCluster(u32 cluster){
            return (cluster >= CLUSTER_FIRST) && (cluster <= this->fat.lastCluster);
        }

        private:
        bool WriteFatEntry(u32 cluster, u32 val);
    };

} // namespace FAT

#endif