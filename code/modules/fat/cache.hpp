#ifndef _FAT_CACHE_
#define _FAT_CACHE_

#include <kamek.hpp>
#include <modules/disc_io/disc_io.hpp>
#include <modules/fat/bit_ops.hpp>

namespace FAT
{
    class CacheEntry
    {
        public:
        u32 sector;
        u32 count;
        u32 lastAccess;
        bool dirty;
        u8 * cache;
    };

    class Cache
    {
        public:
        const DISC_INTERFACE * disc;
        u32 endOfPartition;
        u32 numberOfPages;
        u32 sectorsPerPage;
        u32 bytesPerSector;
        CacheEntry entries[];

        ~Cache();

        static Cache * CreateCache(u8 * space, size_t size, const DISC_INTERFACE * interface, u32 endOfPartition, u32 bytesPerSector);

        bool ReadPartialSector(void * buffer, u32 sector, u32 offset, size_t size);
        bool ReadLittleEndianValue(u32 * value, u32 sector, u32 offset, s32 size);
        bool WritePartialSector(const void * buffer, u32 sector, u32 offset, size_t size);
        bool WriteLittleEndianValue(const u32 value, u32 sector, u32 offset, s32 size);

        bool EraseWritePartialSector(const void * buffer, u32 sector, u32 offset, size_t size);
        bool ReadSectors(u32 sector, u32 sectorCount, void * buffer);

        bool WriteSectors(u32 sector, u32 sectorCount, const void * buffer);

        void Invalidate();

        inline bool ReadSector(void * buffer, u32 sector)
        {
            return this->ReadPartialSector(buffer,sector,0,this->bytesPerSector);
        }

        inline bool WriteSector(const void * buffer, u32 sector)
        {
            return this->WritePartialSector(buffer,sector,0,this->bytesPerSector);
        }

        private:
        bool Flush();
        CacheEntry * GetPage(u32 sector);
       
    };
}


#endif