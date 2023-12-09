#pragma once
#include <kamek.hpp>
#include <core/rvl/os/mutex.hpp>
#include <vendor/ff/ff.h>

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

    OSMutex m_mutex;
    FATFS m_fs;
    bool m_ok;
};
