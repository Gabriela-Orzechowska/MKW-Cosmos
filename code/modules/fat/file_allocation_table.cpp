#include "partition.hpp"

namespace FAT
{
    u32 Partition::NextCluster(u32 cluster)
    {
        u32 nextCluster = CLUSTER_FREE;

        if(cluster == CLUSTER_FREE) return CLUSTER_FREE;

        u32 sector;
        s32 offset;
        switch(this->filesysType)
        {
            case FS_UNKNOWN:
                return CLUSTER_ERROR;
            case FS_FAT12:
                sector = this->fat.fatStart + (((cluster * 3) / 2) / this->bytesPerSector);
                offset = ((cluster * 3)/ 2) % this->bytesPerSector;

                this->cache->ReadLittleEndianValue(&nextCluster, sector,offset,sizeof(u8));

                offset++;

                if(offset >= this->bytesPerSector){
                    offset = 0;
                    sector++;
                }
                u32 nextCluster2 = 0;

                this->cache->ReadLittleEndianValue(&nextCluster2, sector, offset, sizeof(u8));
                nextCluster |= (nextCluster2 << 8);

                if(cluster & 0x1) nextCluster = nextCluster >> 4;
                else nextCluster &= 0xFFF;

                if(nextCluster >= 0x0FF7) nextCluster = CLUSTER_EOF;
                break;

            case FS_FAT16:
                sector = this->fat.fatStart + ((cluster << 1) / this->bytesPerSector);
                offset = (cluster % (this->bytesPerSector >> 1)) << 1;

                this->cache->ReadLittleEndianValue(&nextCluster, sector, offset, sizeof(u16));

                if(nextCluster >= 0xFFF7) nextCluster = CLUSTER_EOF;
                break;
            case FS_FAT32:
                sector = this->fat.fatStart + ((cluster << 2) / this->bytesPerSector);
                offset = (cluster % (this->bytesPerSector >> 2)) << 2;

                this->cache->ReadLittleEndianValue(&nextCluster, sector, offset, sizeof(u32));
                if (nextCluster >= 0x0FFFFFF7) nextCluster = CLUSTER_EOF;
                break;
            default:
                return CLUSTER_ERROR;
        }
        return nextCluster;
    }

    bool Partition::WriteFatEntry(u32 cluster, u32 val)
    {
        if((cluster < CLUSTER_FIRST) || (cluster > this->fat.lastCluster)) return false;

        u32 sector;
        s32 offset;
        u32 oldVal;

        switch(this->filesysType)
        {
            case FS_UNKNOWN:
                return false;
            case FS_FAT12:
                sector = this->fat.fatStart + (((cluster * 3) / 2) / this->bytesPerSector);
                offset = ((cluster * 3)/ 2) % this->bytesPerSector;

                if(cluster & 0x01){
                    this->cache->ReadLittleEndianValue(&oldVal, sector, offset, sizeof(u8));

                    val = (val << 4) | (oldVal & 0x0F);

                    this->cache->WriteLittleEndianValue(val & 0xFF, sector, offset, sizeof(u8));
                    offset++;
                    if(offset >= this->bytesPerSector){
                        offset= 0;
                        sector++;
                    }

                    this->cache->WriteLittleEndianValue((val >> 8) & 0xFF, sector, offset, sizeof(u8));
                }
                else{
                    this->cache->WriteLittleEndianValue(val, sector, offset, sizeof(u8));

                    offset++;
                    if(offset >= this->bytesPerSector){
                        offset= 0;
                        sector++;
                    }

                    this->cache->ReadLittleEndianValue(&oldVal, sector, offset, sizeof(u8));

                    val = ((val >> 8) & 0x0F) | (oldVal & 0xF0);\

                    this->cache->WriteLittleEndianValue(val, sector, offset, sizeof(u8));
                }
                break;

            case FS_FAT16:
                sector = this->fat.fatStart + ((cluster << 1) / this->bytesPerSector);
                offset = (cluster % (this->bytesPerSector >> 1)) << 1;

                this->cache->WriteLittleEndianValue(val, sector, offset, sizeof(u16));
                break;

            case FS_FAT32:
                sector = this->fat.fatStart + ((cluster << 2) / this->bytesPerSector);
                offset = (cluster % (this->bytesPerSector >> 2)) << 2;

                this->cache->WriteLittleEndianValue(val, sector, offset, sizeof(u32));
                break;
            default:
                return false;
        }
        return true;
    }

    u32 Partition::LinkFreeCluster(u32 cluster)
    {
        u32 lastCluster = this->fat.lastCluster;
        bool loopedAroundFAT = false;

        if(cluster > lastCluster) return CLUSTER_ERROR;

        u32 curLink = this->NextCluster(cluster);
        if((curLink >= CLUSTER_FIRST) && (curLink <= lastCluster)) return curLink;

        u32 firstFree = this->fat.firstFree;
        if(firstFree < CLUSTER_FIRST) return CLUSTER_FIRST;

        while(this->NextCluster(firstFree) != CLUSTER_FREE)
        {
            firstFree++;
            if(firstFree > lastCluster){
                if(loopedAroundFAT){
                    this->fat.firstFree = firstFree;
                    return CLUSTER_ERROR;
                }
                else{
                    firstFree = CLUSTER_FIRST;
                    loopedAroundFAT = true;
                }
            }
        }

        this->fat.firstFree = firstFree;
        if(this->fat.numberFreeCluster) this->fat.numberFreeCluster--;
        this->fat.numberLastAllocCluster = firstFree;

        if((cluster >= CLUSTER_FIRST) && (cluster <= lastCluster)){
            this->WriteFatEntry(cluster, firstFree);
        }

        this->WriteFatEntry(firstFree, CLUSTER_EOF);
        return firstFree;
    }

    u32 Partition::LinkFreeClusterCleared(u32 cluster)
    {
        u32 newCluster = this->LinkFreeCluster(cluster);
        if(newCluster == CLUSTER_FREE || newCluster == CLUSTER_ERROR) return CLUSTER_ERROR;

        for(int i = 0; i < this->sectorsPerCluster; i++){
            this->cache->WriteSectors(this->ClusterToSector(newCluster) + i, 1, NULL);
        }
        return newCluster;
    }

    bool Partition::ClearLinks(u32 cluster)
    {
        if((cluster < CLUSTER_FIRST) || (cluster > this->fat.lastCluster)) return false;

        if(cluster < this->fat.firstFree) this->fat.firstFree = cluster;

        u32 nextCluster;
        while((cluster != CLUSTER_EOF) && (cluster != CLUSTER_FREE) && (cluster != CLUSTER_ERROR)){
            nextCluster = this->NextCluster(cluster);

            this->WriteFatEntry(cluster, CLUSTER_FREE);

            if(this->fat.numberFreeCluster < (this->sectorCount/this->sectorsPerCluster)) this->fat.numberFreeCluster++;

            cluster = nextCluster;
        }
        return true;
    }

    u32 Partition::TrimChain(u32 startCluster, u32 length)
    {
        if(length == 0){
            this->ClearLinks(startCluster);
            return CLUSTER_FREE;
        }

        length--;
        u32 nextCluster = this->NextCluster(startCluster);
        while((length > 0) && (nextCluster != CLUSTER_FREE) && (nextCluster != CLUSTER_EOF)){
            length--;
            startCluster = nextCluster;
            nextCluster = this->NextCluster(startCluster);
        }

        if(nextCluster != CLUSTER_FREE && nextCluster != CLUSTER_EOF){
            this->ClearLinks(nextCluster);
        }
        this->WriteFatEntry(startCluster, CLUSTER_EOF);
        return startCluster;
    }

    u32 Partition::LastCluster(u32 cluster){
        while((this->NextCluster(cluster) != CLUSTER_FREE) && (this->NextCluster(cluster) != CLUSTER_EOF)){
            cluster = this->NextCluster(cluster);
        }
        return cluster;
    }

    u32 Partition::FreeClusterCount(){
        u32 count = 0;

        for(u32 curCluster = CLUSTER_FIRST; curCluster <= this->fat.lastCluster; curCluster++){
            if(this->NextCluster(curCluster) == CLUSTER_FREE){
                count++;
            }
        }
        return count;
    }

} // namespace FAT
