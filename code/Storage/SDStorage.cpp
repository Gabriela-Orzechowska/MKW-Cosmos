#include <Storage/SDStorage.hpp>
#include <Storage/disc/SD.hpp>
#include <vendor/ff/ff.h>
#include <core/System/RKSystem.hpp>
#include <Debug/Draw/DebugDraw.hpp>

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

const char sd_fd[] __attribute__((aligned(0x20))) = "/dev/sdio/slot0";

static CosmosDebug::DebugMessage sdStatusMessage(false);

bool SDStorage::Init()
{
    s32 fd;

    if(sdfd < 0){
        fd = Cosmos::Open((char *) sd_fd, IOS::MODE_NONE);
    }
    else fd = sdfd;
    if(fd < 0)
    {
        sdStatusMessage.SetMessage("[FAT] Failed to open /dev/sdio/slot0");
        sdStatusMessage.DisplayForX(60);
        CosmosLog("Failed to open /dev/sdio/slot0");
        return false;
    }

    CosmosLog("Opened SD interface, ID: %i\n", fd);

    if(!SD_Reset(fd)) {
        CosmosLog("Failed to reset SD Card\n");
        sdStatusMessage.SetMessage("[FAT] Failed to reset SD Card");
        sdStatusMessage.DisplayForX(60);
        return false;
    }
    u32 status;
    if(!SD_GetStatus(&status)) 
    {
        CosmosLog("Unable to get status\n");
        sdStatusMessage.SetMessage("[FAT] Unable to get status");
        sdStatusMessage.DisplayForX(60);
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INSERTED)){
        CosmosLog("SD card not inserted\n");
        sdStatusMessage.SetMessage("[FAT] SD card not inserted");
        sdStatusMessage.DisplayForX(60);
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INITIALIZED))
    {
        CosmosLog("Could not initialize filesystem... Retrying...\n");
        sdStatusMessage.SetMessage("[FAT] Could not initialize filesystem... Retrying...");
        sdStatusMessage.DisplayForX(60);
        bool ret = SD_Reinitialize();
        SD_GetStatus(&status);

        if(!(status & SDIO_STATUS_CARD_INITIALIZED) || !ret){
            CosmosLog("Retry initialization has failed...");
            sdStatusMessage.SetMessage("[FAT] Retry initialization has failed...");
            sdStatusMessage.DisplayForX(60);
            return false;
        } 
        CosmosLog("Success\n");
        sdStatusMessage.SetMessage("[FAT] Init retry success");
        sdStatusMessage.DisplayForX(60);
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
    sdStatusMessage.SetMessage("[FAT] Successfully initialized SD card");
    sdStatusMessage.DisplayForX(60);
    
    FRESULT result = f_mount(&device->m_fs, L"", 1);
    
    if(result != FR_OK)
    {
        char strbuffer[0x40];
        CosmosLog("Couldn't initialize FAT\n");
        sdStatusMessage.SetMessage("[FAT] Couldn't initialize FAT");
        sdStatusMessage.DisplayForX(60);
        StorageDevice::currentDevice = nullptr;
        return false;
    }
    CosmosLog("Mounted FAT\n");
    sdStatusMessage.SetMessage("[FAT] Mounted FAT");
    sdStatusMessage.DisplayForX(60);

    return true;
}
