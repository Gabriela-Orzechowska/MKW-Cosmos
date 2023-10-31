#pragma once

#include <kamek.hpp>
#include <modules/fat/common.hpp>
#include <modules/fat/partition.hpp>

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
    namespace fat
    {
        u32 NextCluster(PARTITION * partition, u32 cluster);
        u32 LinkFreeCluster(PARTITION* partition, u32 cluster);
        u32 LinkFreeClusterCleared (PARTITION* partition, u32 cluster);
        bool ClearLinks(PARTITION * partition, u32 cluster);
        u32 TrimChain(PARTITION * partition, u32 startCluster, u32 chainLenght);
        u32 LastCluster(PARTITION * partition, u32 cluster);
        s32 FreeClusterCount(PARTITION * partition);

        static inline sec_t ClusterToSector(PARTITION * partition, u32 cluster)
        {
            return (cluster >= CLUSTER_FIRST) ? ((cluster - CLUSTER_FIRST) * (sec_t)partition->sectorsPerCluster) + partition->dataStart : partition->rootDirStart;
        }
        static inline bool IsValidCluster(PARTITION * partition, u32 cluster)
        {
            return (cluster >= CLUSTER_FIRST) && (cluster <= partition->fat.lastCluster /* This will catch CLUSTER_ERROR */);
        }
    } // namespace fat
    
} // namespace FAT
