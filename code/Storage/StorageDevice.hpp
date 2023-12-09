#pragma once
#include <kamek.hpp>

class StorageDevice
{
    public:
    static StorageDevice * GetDevice();
    static StorageDevice * currentDevice;

    virtual u32 diskSectorSize() = 0;
    virtual bool diskRead(u32 sector, u32 count, void * buffer) = 0;
    virtual bool diskWrite(u32 sector, u32 count, const void * buffer) = 0;
    virtual bool diskErase(u32 sector, u32 count) = 0;
    virtual bool diskSync() = 0;
    virtual u32 diskGetMessageId() = 0;
};

// For FatFs

static u32 StorageDevice_SectorSize();
static bool StorageDevice_Read(u32 sector, u32 count, void * buffer);
static bool StorageDevice_Write(u32 sector, u32 count, const void * buffer);
static bool StorageDevice_Erase(u32 sector, u32 count);
static bool StorageDevice_Sync();
static u32 StorageDevice_GetMessageId();