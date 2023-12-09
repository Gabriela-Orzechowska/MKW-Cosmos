#include <Storage/StorageDevice.hpp>

StorageDevice * StorageDevice::currentDevice = nullptr;

StorageDevice * StorageDevice::GetDevice()
{
    return currentDevice;
}

static u32 StorageDevice_SectorSize()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskSectorSize();
}

static bool StorageDevice_Read(u32 sector, u32 count, void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskRead(sector,count,buffer);
}

static bool StorageDevice_Write(u32 sector, u32 count, const void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskWrite(sector,count,buffer);
}

static bool StorageDevice_Erase(u32 sector, u32 count)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskErase(sector,count);
}

static bool StorageDevice_Sync()
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskSync();
}

static u32 StorageDevice_GetMessageId()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskGetMessageId();
}