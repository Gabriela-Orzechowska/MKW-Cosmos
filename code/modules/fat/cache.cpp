#include <modules/fat/cache.hpp>

#define SECTORS_PER_PAGE 8
#define CACHE_FREE 0xFFFFFFFF

namespace FAT
{
    Cache::~Cache(){
        this->Invalidate();
    }

    static u32 accessCounter = 0;

    static u32 accessTime(){
        return ++accessCounter;
    }

    CacheEntry * Cache::GetPage(u32 sector)
    {
        bool foundFree = false;
        u32 oldUsed = 0;
        u32 oldAccess = ~0;
        for(int i = 0; i < this->numberOfPages; i++)
        {
            if (sector >= this->entries[i].sector && sector < (this->entries[i].sector + this->entries[i].count))
            {
                this->entries[i].lastAccess = accessTime();
                return &this->entries[i];
            }

            if(!foundFree && (this->entries[i].sector == CACHE_FREE || this->entries[i].lastAccess < oldAccess))
            {
                if(this->entries[i].sector == CACHE_FREE) foundFree = true;
                oldUsed = i;
                oldAccess = this->entries[i].lastAccess;
            }
        }

        CacheEntry * entry = &this->entries[oldUsed];

        if(!foundFree && entry->dirty)
        {
            if(!this->disc->writeSectors(entry->sector,entry->count,entry->cache)) return nullptr;
            entry->dirty = false;
        }

        sector = (sector/sectorsPerPage)*sectorsPerPage;
        u32 next_page = sector + sectorsPerPage;
        if(next_page > this->endOfPartition) next_page = this->endOfPartition;
    
        if(!this->disc->readSectors(sector,next_page-sector,entry->cache)) return nullptr;

        entry->sector = sector;
        entry->count = next_page-sector;
        entry->lastAccess = accessTime();

        return entry;
    }

    bool Cache::ReadSectors(u32 sector, u32 count, void * buffer)
    {
        u8 * dest = (u8 *) buffer;
        while(count > 0)
        {
            CacheEntry * entry = this->GetPage(sector);
            if(entry == nullptr) return false;

            u32 sec = sector - entry->sector;
            u32 secs_to_read = entry->count - sector;
            if(secs_to_read>count) secs_to_read = count;

            memcpy(dest,entry->cache + (sec*this->bytesPerSector), (secs_to_read*this->bytesPerSector));
            dest += (secs_to_read*this->bytesPerSector);
            sector += secs_to_read;
            count -=secs_to_read;
        }
        return true;
    }

    bool Cache::ReadPartialSector(void * buffer, u32 sector, u32 offset, size_t size)
    {
        if(offset + size > this->bytesPerSector) return false;
        CacheEntry * entry = this->GetPage(sector);
        if(entry == nullptr) return false;

        u32 sec = sector - entry->sector;
        memcpy(buffer,entry->cache + ((sec * this->bytesPerSector) + offset),size);

        return true;
    }

    bool Cache::ReadLittleEndianValue(u32 * value, u32 sector, u32 offset, s32 size)
    {
        u8 buffer[4];
        if(!this->ReadPartialSector(buffer, sector, offset,size)) return false;

        switch(size)
        {
            case 1: *value = buffer[0]; break;
            case 2: *value = (u16) (buffer[0] | buffer[1] << 8); break;
            case 4: *value = (u32) (buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24); break;
            default: return false;
        }
        return true;
    }

    bool Cache::WritePartialSector(const void * buffer, u32 sector, u32 offset, size_t size)
    {
        if(offset + size > this->bytesPerSector) return false;
        CacheEntry * entry = this->GetPage(sector);
        if(entry == nullptr) return false;

        u32 sec = sector - entry->sector;
        memcpy(entry->cache + ((sec * this->bytesPerSector) + offset),buffer,size);
        entry->dirty = true;
        return true;
    }

    bool Cache::WriteLittleEndianValue(const u32 value, u32 sector, u32 offset, s32 size)
    {
        u8 buffer[4] = {0,0,0,0};
        switch(size)
        {
            case 1: buffer[0] = value;break;
            case 2: 
                buffer[0] = (value >> 8) & 0xFF;
                buffer[1] = value & 0xFF;
                break;
            case 3:
                buffer[0] = (value >> 24) & 0xFF;
                buffer[1] = (value >> 16) & 0xFF;
                buffer[2] = (value >> 8) & 0xFF;
                buffer[3] = value & 0xFF;
                break;
            default: return false;
        }
        return this->WritePartialSector(buffer, sector, offset,size);
    }

    bool Cache::EraseWritePartialSector(const void * buffer, u32 sector, u32 offset, size_t size)
    {
        if(offset + size > this->bytesPerSector) return false;
        CacheEntry * entry = this->GetPage(sector);
        if(entry == nullptr) return false;

        u32 sec = sector - entry->sector;
        memset(entry->cache + (sec*this->bytesPerSector), 0, this->bytesPerSector);
        memcpy(entry->cache + ((sec*this->bytesPerSector) + offset),buffer,size);
        entry->dirty = true;
        return true;
    }

    bool Cache::WriteSectors(u32 sector, u32 sectorCount, const void * buffer)
    {
        const u8 * src = (const u8 *) buffer;

        while(sectorCount > 0)
        {
            CacheEntry * entry = this->GetPage(sector);
            if(entry == nullptr) return false;
            u32 sec = sector - entry->sector;
            u32 secs_to_write = entry->count - sec;
            if(secs_to_write > sectorCount) secs_to_write = sectorCount;

            if (src != nullptr) {
                memcpy(entry->cache + (sec*this->bytesPerSector),src,(secs_to_write*this->bytesPerSector));
                src += (secs_to_write*this->bytesPerSector);
            } else {
                memset(entry->cache + (sec*this->bytesPerSector),0,(secs_to_write*this->bytesPerSector));
            }
            
            sector += secs_to_write;
            sectorCount -= secs_to_write;

            entry->dirty = true;
        }
        return true;
    }

    bool Cache::Flush()
    {
        for(int i = 0; i < this->numberOfPages; i++)
        {
            CacheEntry entry = this->entries[i];
            if(entry.dirty)
            {
                if(!this->disc->writeSectors(entry.sector, entry.count, entry.cache))
                {
                    return false;
                }
            }
            this->entries[i].dirty = false;
        }
        return true;
    }

    void Cache::Invalidate()
    {
        this->Flush();
        for(int i = 0; i < this->numberOfPages; i++)
        {
            this->entries[i].sector = CACHE_FREE;
            this->entries[i].lastAccess = 0;
            this->entries[i].count = 0;
            this->entries[i].dirty = false;
        }
    }

    Cache * Cache::GetCache(u8 * space, size_t size, const DISC_INTERFACE * interface, u32 endOfPartition, u32 bytesPerSector)
    {
        if(size < sizeof(Cache)) return nullptr;
        u32 numberOfPages = (size - sizeof(Cache) - 32) / (sizeof(CacheEntry) + bytesPerSector * SECTORS_PER_PAGE);
        if(numberOfPages < 2) return nullptr;
        
        Cache * cache;
        cache = (Cache *) space;
        if(cache == nullptr) return nullptr;

        cache += sizeof(Cache);

        cache->disc = interface;
        cache->endOfPartition = endOfPartition;
        cache->numberOfPages = numberOfPages;
        cache->sectorsPerPage = SECTORS_PER_PAGE;
        cache->bytesPerSector = bytesPerSector;

        CacheEntry * entries = cache->entries;
        space += sizeof(CacheEntry) * numberOfPages;
        space += (-(u32)space & 0x1F);

        for (int i = 0; i < numberOfPages; i++) {
            entries[i].sector = CACHE_FREE;
            entries[i].count = 0;
            entries[i].lastAccess = 0;
            entries[i].dirty = false;
            entries[i].cache = space;
            
            space += bytesPerSector * SECTORS_PER_PAGE;
        }
        return cache;
    }
} 
