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
        fd = DX::Open((char *) sd_fd, IOS::MODE_NONE);
    }
    else fd = sdfd;
    if(fd < 0)
    {
        DXLog("Failed to open /dev/sdio/slot0");
        return false;
    }

    DXLog("Opened SD interface, ID: %i\n", fd);

    if(!SD_Reset(fd)) {
        DXLog("Failed to reset SD Card\n");
        return false;
    }
    u32 status;
    if(!SD_GetStatus(&status)) 
    {
        DXLog("Unable to get status\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INSERTED)){
        DXLog("SD card not inserted\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INITIALIZED))
    {
        DXLog("Could not initialize filesystem... Retrying...\n");
        bool ret = SD_Reinitialize();
        SD_GetStatus(&status);

        if(!(status & SDIO_STATUS_CARD_INITIALIZED) || !ret){
            QuickFatal("Retry initialization has failed...");
            return false;
        } 
        DXLog("Success\n");
    }

    //sdhc = !!(status & SDIO_STATUS_CARD_SDHC);
    if(!SD_Enable4bitBus()){
        DXLog("Failed to enable 4-bit mode\n");
        return false;
    }

    if(!SD_SetClock(1))
    {
        DXLog("Unable to set the clock\n");
        return false;
    }

    if(!SD_Select())
    {
        DXLog("Unable to select the sd card\n");
        return false;
    }

    if(!SD_SetCardBlockSize(SECTOR_SIZE)){
        SD_Deselect();
        DXLog("Unable to set block size\n");
        return false;
    }

    if(!SD_EnableCard4BitBus){
        SD_Deselect();
        DXLog("Unable to enable card 4 bit bus\n");
        return false;
    }

    SD_Deselect();
    StorageDevice * device = new(RKSystem::mInstance.EGGSystem) SDStorage;
    StorageDevice::currentDevice = device;

    DXLog("Successfully initialized SD card\n");
    
    FRESULT result = f_mount(&device->m_fs, L"", 1);
    
    if(result != FR_OK)
    {
        char strbuffer[0x40];
        DXLog("Couldn't initialize FAT\nError: %i\nStatus: %x", result, status);
        StorageDevice::currentDevice = nullptr;
        return false;
    }
    DXLog("Initialized FAT\n");

    result = f_mkdir(L"/AAAAA");

    return true;
}

void InitSDCard()
{
    SDStorage::Init();
    return;
}

static BootHook bhSDCard(InitSDCard, FIRST);