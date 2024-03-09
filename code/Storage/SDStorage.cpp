#include <Storage/SDStorage.hpp>
#include <Storage/disc/SD.hpp>
#include <vendor/ff/ff.h>
#include <core/System/RKSystem.hpp>

u32 SDStorage::diskSectorSize()
{
    return SD_SectorSize();
}

bool SDStorage::diskRead(u32 sector, u32 count, void * buffer)
{
    return SD_Read(sector, count, buffer);
}

bool SDStorage::diskWrite(u32 sector, u32 count, const void * buffer)
{
    return SD_Write(sector,count,buffer);
}

bool SDStorage::diskErase(u32 sector, u32 count)
{
    return SD_Erase();
}

bool SDStorage::diskSync()
{
    return true;
}

u32 SDStorage::diskGetMessageId()
{
    return SD_GetMessageId();
}

void QuickFatal(char * string)
{
    u32 black = 0;
    u32 white = ~0;
    OSFatal(&white, &black, string);
}

const char sd_fd[] __attribute__ ((aligned(0x20))) = "/dev/sdio/slot0";

bool SDStorage::Init()
{
    s32 fd;

    if(sdfd < 0){
        fd = Cosmos::Open((char *) sd_fd, IOS::MODE_NONE);
    }
    else fd = sdfd;
    if(fd < 0)
    {
        CosmosLog("Failed to open /dev/sdio/slot0");
        return false;
    }

    CosmosLog("Opened SD interface, ID: %i\n", fd);

    if(!SD_Reset(fd)) {
        CosmosLog("Failed to reset SD Card\n");
        return false;
    }
    u32 status;
    if(!SD_GetStatus(&status)) 
    {
        CosmosLog("Unable to get status\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INSERTED)){
        CosmosLog("SD card not inserted\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INITIALIZED))
    {
        CosmosLog("Could not initialize filesystem... Retrying...\n");
        bool ret = SD_Reinitialize();
        SD_GetStatus(&status);

        if(!(status & SDIO_STATUS_CARD_INITIALIZED) || !ret){
            CosmosLog("Retry initialization has failed...");
            return false;
        } 
        CosmosLog("Success\n");
    }

    //sdhc = !!(status & SDIO_STATUS_CARD_SDHC);
    if(!SD_Enable4bitBus()){
        CosmosLog("Failed to enable 4-bit mode\n");
        return false;
    }

    if(!SD_SetClock(1))
    {
        CosmosLog("Unable to set the clock\n");
        return false;
    }

    if(!SD_Select())
    {
        CosmosLog("Unable to select the sd card\n");
        return false;
    }

    if(!SD_SetCardBlockSize(SECTOR_SIZE)){
        SD_Deselect();
        CosmosLog("Unable to set block size\n");
        return false;
    }

    if(!SD_EnableCard4BitBus){
        SD_Deselect();
        CosmosLog("Unable to enable card 4 bit bus\n");
        return false;
    }

    SD_Deselect();
    StorageDevice * device = new(RKSystem::mInstance.EGGSystem) SDStorage;
    StorageDevice::currentDevice = device;

    CosmosLog("Successfully initialized SD card\n");
    
    FRESULT result = f_mount(&device->m_fs, L"", 1);
    
    if(result != FR_OK)
    {
        char strbuffer[0x40];
        CosmosLog("Couldn't initialize FAT\n");
        StorageDevice::currentDevice = nullptr;
        return false;
    }
    CosmosLog("Mounted FAT\n");

    return true;
}
