#pragma once

#include <modules/fat/directory.hpp>
#include <modules/fat/partition.hpp>

namespace FAT
{
    // Directory entry codes
    #define DIR_ENTRY_LAST 0x00
    #define DIR_ENTRY_FREE 0xE5

    #define LAST_LFN_POS (19*13)
    #define LAST_LFN_POS_CORRECTION (MAX_LFN_LENGTH-15)

    enum LFN_offset {
        LFN_offset_ordinal = 0x00,	// Position within LFN
        LFN_offset_char0 = 0x01,
        LFN_offset_char1 = 0x03,
        LFN_offset_char2 = 0x05,
        LFN_offset_char3 = 0x07,
        LFN_offset_char4 = 0x09,
        LFN_offset_flag = 0x0B,	// Should be equal to ATTRIB_LFN
        LFN_offset_reserved1 = 0x0C,	// Always 0x00
        LFN_offset_checkSum = 0x0D,	// Checksum of short file name (alias)
        LFN_offset_char5 = 0x0E,
        LFN_offset_char6 = 0x10,
        LFN_offset_char7 = 0x12,
        LFN_offset_char8 = 0x14,
        LFN_offset_char9 = 0x16,
        LFN_offset_char10 = 0x18,
        LFN_offset_reserved2 = 0x1A,	// Always 0x0000
        LFN_offset_char11 = 0x1C,
        LFN_offset_char12 = 0x1E
    };
    static const int LFN_offset_table[13]={0x01,0x03,0x05,0x07,0x09,0x0E,0x10,0x12,0x14,0x16,0x18,0x1C,0x1E};

    #define LFN_END 0x40
    #define LFN_DEL 0x80

    static const char ILLEGAL_ALIAS_CHARACTERS[] = "\\/:;*?\"<>|&+,=[] ";
    static const char ILLEGAL_LFN_CHARACTERS[] = "\\/:*?\"<>|";

    /*
    s32 Partition::Stat(const char * path, stat * st)
    {
        if(this == nullptr) return -1;
        if(strchr(path, ':') != NULL) path = strchr(path,':') + 1;
        if(strchr(path, ':') != NULL) return -1;

        Lock::Lock(&this->lock);

        Directory dir;

        const char * end = (const char *) NULL;

        if(!this->EntryFromPath(&dir, path, end)){
            Lock::Unlock(&this->lock);
            return -1;
        }

        this->EntryStat(&dir, st);
        Lock::Unlock(&this->lock);
        return 0;
    }

    s32 Partition::Unlink(const char * path)
    {
        if(this == nullptr) return -1;
        if(this->readOnly) return -1;

        if(strchr(path, ':') != NULL) path = strchr(path,':') + 1;
        if(strchr(path, ':') != NULL) return -1;

        Lock::Lock(&this->lock);
        Directory dir;
        if(!this->EntryFromPath(&dir, path, NULL)){
            Lock::Unlock(&this->lock);
            return -1;
        }

        u32 cluster = this->EntryGetCluster(dir.entryData);
        if(dir.IsDirectory())
        {
            Directory content;
            u32 nextEntry = this->GetFirstEntry(&content, cluster);

            while(nextEntry)
            {
                if(!content.IsDot()){
                    Lock::Unlock(&this->lock);
                    return -1;
                }
                nextEntry = this->GetNextEntry(&content);
            }
        }
        bool err = false;
        if(!this->IsValidCluster(cluster)){
            if(!this->ClearLinks(cluster))
                err = true;
        }
        if(!this->RemoveEntry(&dir)) err = true;
        if(!this->cache->Flush()) err = true;

        Lock::Unlock(&this->lock);
        return err ? 0 : -1;
    }

    bool Partition::Chdir(const char * path)
    {
        if(this == nullptr) return -1;

        if(strchr(path, ':') != NULL) path = strchr(path,':') + 1;
        if(strchr(path, ':') != NULL) return -1;

        return false;

    }
    */
} // namespace FAT
