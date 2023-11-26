#include "partition.hpp"
#include "bit_ops.hpp"

enum BPB {
	BPB_jmpBoot = 0x00,
	BPB_OEMName = 0x03,
	// BIOS Parameter Block
	BPB_bytesPerSector = 0x0B,
	BPB_sectorsPerCluster = 0x0D,
	BPB_reservedSectors = 0x0E,
	BPB_numFATs = 0x10,
	BPB_rootEntries = 0x11,
	BPB_numSectorsSmall = 0x13,
	BPB_mediaDesc = 0x15,
	BPB_sectorsPerFAT = 0x16,
	BPB_sectorsPerTrk = 0x18,
	BPB_numHeads = 0x1A,
	BPB_numHiddenSectors = 0x1C,
	BPB_numSectors = 0x20,
	// Ext BIOS Parameter Block for FAT16
	BPB_FAT16_driveNumber = 0x24,
	BPB_FAT16_reserved1 = 0x25,
	BPB_FAT16_extBootSig = 0x26,
	BPB_FAT16_volumeID = 0x27,
	BPB_FAT16_volumeLabel = 0x2B,
	BPB_FAT16_fileSysType = 0x36,
	// Bootcode
	BPB_FAT16_bootCode = 0x3E,
	// FAT32 extended block
	BPB_FAT32_sectorsPerFAT32 = 0x24,
	BPB_FAT32_extFlags = 0x28,
	BPB_FAT32_fsVer = 0x2A,
	BPB_FAT32_rootClus = 0x2C,
	BPB_FAT32_fsInfo = 0x30,
	BPB_FAT32_bkBootSec = 0x32,
	// Ext BIOS Parameter Block for FAT32
	BPB_FAT32_driveNumber = 0x40,
	BPB_FAT32_reserved1 = 0x41,
	BPB_FAT32_extBootSig = 0x42,
	BPB_FAT32_volumeID = 0x43,
	BPB_FAT32_volumeLabel = 0x47,
	BPB_FAT32_fileSysType = 0x52,
	// Bootcode
	BPB_FAT32_bootCode = 0x5A,
	BPB_bootSig_55 = 0x1FE,
	BPB_bootSig_AA = 0x1FF
};

enum FSIB
{
	FSIB_SIG1 = 0x00,
	FSIB_SIG2 = 0x1e4,
	FSIB_numberOfFreeCluster = 0x1e8,
	FSIB_numberLastAllocCluster = 0x1ec,
	FSIB_bootSig_55 = 0x1FE,
	FSIB_bootSig_AA = 0x1FF
};

static const char FAT_SIG[3] = {'F', 'A', 'T'};
static const u32 FS_INFO_SIG1 = 0x41615252; /* 'RRaA' in little endian */
static const u32 FS_INFO_SIG2 = 0x61417272; /* 'rrAa' in little endian */

inline static u32 u8array_to_u32(const u8 * ptr, int offset)
{
    (u32) (ptr[offset] | ptr[1+offset] << 8 | ptr[2+offset] << 16 | ptr[3+offset] << 24);
}

inline static u16 u8array_to_u16(const u8 * ptr, int offset)
{
    (u16) (ptr[offset] | ptr[1+offset] << 8);
}

namespace FAT
{
    u32 FindFirstValidPartition(const DISC_INTERFACE * disc, u8 * sectorBuffer)
    {
        u8 partTable[16*4];
        u8 * ptr;

        if(!disc->readSectors(0,1,sectorBuffer)) return 0;
        memcpy(partTable,sectorBuffer+0x1BE,16*4);

        ptr = partTable;

        for(int i = 0; i < 4; i++,ptr+=16)
        {
            u32 partLba = u8array_to_u32(ptr,0x8);
            if(!memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)) || !memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))){
                return partLba;
            }

            if(ptr[4] == 0) continue;

            if(ptr[4] == 0x0F){
                u32 partLba2 = partLba;
                u32 nextLba2 = 0;

                for(int j = 0; j < 8; j++)
                {
                    if(!disc->readSectors(partLba+nextLba2,1,sectorBuffer)) return 0;
                    partLba2 = partLba + nextLba2 + u8array_to_u32(sectorBuffer, 0x1C6);

                    if(!disc->readSectors(partLba2, 1, sectorBuffer)) return 0;

                    if(!memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)) || !memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))){
                        return partLba2;
                    }
                }
            }
            else{
                if(!disc->readSectors(partLba,1,sectorBuffer)) return 0;
                if(!memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)) || !memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))){
                    return partLba;
                }
            }
        }
        return 0;
    }

    bool Partition::GetPartition(const DISC_INTERFACE * disc, u8 * cacheSpace, size_t cacheSize, u32 startSector)
    {
        u8 * sectorBuffer = cacheSpace;

        if(cacheSize < MAX_SECTOR_SIZE || cacheSpace == NULL) return false;

        if(!disc->readSectors(startSector, 1, sectorBuffer)) return false;

        if((sectorBuffer[BPB_bootSig_55] != 0x55) || sectorBuffer[BPB_bootSig_AA] != 0xAA) return false;

        if(startSector != 0) {}
        else if(!memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG))) {startSector = 0;}
        else if(!memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))) {startSector = 0;}
        else {
            startSector = FindFirstValidPartition(disc, sectorBuffer);
            if(!disc->readSectors(startSector, 1, sectorBuffer)) return NULL;
        }

        if(memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)) && memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))) return false;

        Lock::Init(&this->lock);

        if(!memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG))) strncpy(this->label, (char *)sectorBuffer + BPB_FAT16_volumeLabel, 11);
        else strncpy(this->label, (char *)sectorBuffer + BPB_FAT32_volumeLabel, 11);
        this->label[11] = '\0';
        this->disc = disc;

        this->fat.sectorsPerFat = u8array_to_u16(sectorBuffer, BPB_sectorsPerFAT);
        if(this->fat.sectorsPerFat == 0) this->fat.sectorsPerFat = u8array_to_u32(sectorBuffer, BPB_FAT32_sectorsPerFAT32);

        this->sectorCount = u8array_to_u16(sectorBuffer, BPB_numSectorsSmall);
        if(this->sectorCount == 0) this->sectorCount = u8array_to_u32(sectorBuffer, BPB_numSectors);

        this->bytesPerSector = u8array_to_u16(sectorBuffer, BPB_bytesPerSector);
        if(this->bytesPerSector < MIN_SECTOR_SIZE || this->bytesPerSector > MAX_SECTOR_SIZE) return false;

        this->sectorsPerCluster = sectorBuffer[BPB_sectorsPerCluster];
        this->bytesPerCluster = this->bytesPerSector * this->sectorsPerCluster;
        this->fat.fatStart = startSector + u8array_to_u16(sectorBuffer, BPB_reservedSectors);

        this->rootDirStart = this->fat.fatStart + (sectorBuffer[BPB_numFATs] * this->fat.sectorsPerFat);
        this->dataStart = this->rootDirStart + ((u8array_to_u16(sectorBuffer, BPB_rootEntries) * 0x20) / this->bytesPerSector);
        this->totalSize = ((u64)this->sectorCount - (this->dataStart - startSector)) * (u64)this->bytesPerSector;

        this->fsInfoSector = startSector + (u8array_to_u16(sectorBuffer, BPB_FAT32_fsInfo) ? u8array_to_u16(sectorBuffer, BPB_FAT32_fsInfo) : 1);

        u32 clusterCount = (this->sectorCount - (u32)(this->dataStart - startSector)) /  this->sectorsPerCluster;
        this->fat.lastCluster = clusterCount + CLUSTER_FIRST - 1;
        this->fat.firstFree = CLUSTER_FIRST;
        this->fat.numberFreeCluster = 0;
        this->fat.numberLastAllocCluster = 0;

        this->filesysType = FS_FAT32;
        if(clusterCount < CLUSTERS_PER_FAT12) this->filesysType = FS_FAT12;
        else if(clusterCount < CLUSTERS_PER_FAT16) this->filesysType = FS_FAT16;

        if(this->filesysType != FS_FAT32) this->rootDirCluster = 0;
        else{
            this->rootDirCluster = u8array_to_u32(sectorBuffer, BPB_FAT32_rootClus);
            if(!(sectorBuffer[BPB_FAT32_extFlags] & 0x80)){
                this->fat.fatStart = this->fat.fatStart + (this->fat.sectorsPerFat * (sectorBuffer[BPB_FAT32_extFlags] & 0x0F));
            }
        }

        this->cache = Cache::GetCache(cacheSpace, cacheSize, this->disc, startSector + this->sectorCount, this->bytesPerSector);
        this->cwdCluster = this->rootDirCluster;
        this->readOnly = !((disc->features) & FEATURE_MEDIUM_CANWRITE); 
        this->openFileCount = 0;
        this->firstOpenFile = NULL;
        this->ReadFSInfo();
        return true;
    }

    void Partition::Deinit()
    {
        _FILE_STRUCT * file;
        Lock::Lock(&this->lock);
        file = this->firstOpenFile;
        while(file)
        {
            //TODO
            file = file->nextOpenFile;
        }

        this->WriteFSInfo();
        this->cache->~Cache();
        Lock::Unlock(&this->lock);
        Lock::DeInit(&this->lock);
        return;
    }

    void Partition::CreateFSInfo()
    {
        if(this->readOnly || this->filesysType != FS_FAT32) return;

        if(!this->cache->WriteSector(NULL, this->fsInfoSector)) return;

        Cache * cache = this->cache;
        cache->WriteLittleEndianValue(FS_INFO_SIG1, this->fsInfoSector, FSIB_SIG1, 4);
        cache->WriteLittleEndianValue(FS_INFO_SIG2, this->fsInfoSector, FSIB_SIG2, 4);

        //this->fat.numberFreeCluster = //TODO

        cache->WriteLittleEndianValue(this->fat.numberFreeCluster, this->fsInfoSector, FSIB_numberOfFreeCluster, 4);
        cache->WriteLittleEndianValue(this->fat.numberLastAllocCluster, this->fsInfoSector, FSIB_numberLastAllocCluster, 4);
        cache->WriteLittleEndianValue(0x55, this->fsInfoSector, FSIB_bootSig_55, 1);
        cache->WriteLittleEndianValue(0xAA, this->fsInfoSector, FSIB_bootSig_AA, 1);

        cache->Flush();
        return;
    }

    void Partition::ReadFSInfo()
    {
        u32 buffer;
        if(this->filesysType != FS_FAT32) return;

        Cache * cache = this->cache;
        if(!cache->ReadLittleEndianValue(&buffer, this->fsInfoSector, FSIB_SIG1,4)) return;
        if(buffer != FS_INFO_SIG1) goto createFS;

        if(!cache->ReadLittleEndianValue(&buffer, this->fsInfoSector, FSIB_SIG1,4)) return;
        if(buffer != FS_INFO_SIG2) goto createFS;

        if(!cache->ReadLittleEndianValue(&buffer, this->fsInfoSector, FSIB_numberOfFreeCluster,4)) return;
        if(buffer != 0) goto createFS;

        cache->ReadLittleEndianValue(&this->fat.numberFreeCluster, this->fsInfoSector, FSIB_numberOfFreeCluster, 4);
        cache->ReadLittleEndianValue(&this->fat.numberLastAllocCluster, this->fsInfoSector, FSIB_numberLastAllocCluster, 4);

        return;
        createFS:
        this->CreateFSInfo();
        return;
    }

    void Partition::WriteFSInfo()
    {
        u32 buffer;

        if(this->filesysType != FS_FAT32) return;

        Cache * cache = this->cache;
        if(!cache->ReadLittleEndianValue(&buffer, this->fsInfoSector, FSIB_SIG1,4)) return;
        if(buffer != FS_INFO_SIG1) return;

        if(!cache->ReadLittleEndianValue(&buffer, this->fsInfoSector, FSIB_SIG1,4)) return;
        if(buffer != FS_INFO_SIG2) return;

        cache->WriteLittleEndianValue(this->fat.numberFreeCluster, this->fsInfoSector, FSIB_numberOfFreeCluster, 4);
        cache->WriteLittleEndianValue(this->fat.numberLastAllocCluster, this->fsInfoSector, FSIB_numberLastAllocCluster, 4);

        cache->Flush();
        return;
    }
} // namespace FAT
