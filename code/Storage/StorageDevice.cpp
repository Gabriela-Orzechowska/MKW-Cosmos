#include <Storage/StorageDevice.hpp>
#include <core/rvl/os/OS.hpp>

StorageDevice * StorageDevice::currentDevice = nullptr;

StorageDevice * StorageDevice::GetDevice()
{
    return currentDevice;
}

u32 StorageDevice_SectorSize()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskSectorSize();
}

bool StorageDevice_Read(u32 sector, u32 count, void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) {
        return false;
    }
    return StorageDevice::currentDevice->diskRead(sector,count,buffer);
}

bool StorageDevice_Write(u32 sector, u32 count, const void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskWrite(sector,count,buffer);
}

bool StorageDevice_Erase(u32 sector, u32 count)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskErase(sector,count);
}

bool StorageDevice_Sync()
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskSync();
}

u32 StorageDevice_GetMessageId()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskGetMessageId();
}