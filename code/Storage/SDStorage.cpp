#include <Storage/SDStorage.hpp>
#include <Storage/disc/SD.hpp>

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

bool SDStorage::Init()
{
    s32 fd;

    if(sdfd < 0){
        fd = DX::Open("/dev/sdio/slot0", IOS::MODE_NONE);
    }
    else fd = sdfd;
    if(fd < 0)
    {
        OSReport("[DX] Failed to open /dev/sdio/slot0, error: %i\n", fd);
        return false;
    }
    else OSReport("[DX] Opened SD interface, ID: %i\n", fd);

    if(!SD_Reset(fd)) {
        OSReport("[DX] Failed to reset SD Card");
        return false;
    }
    u32 status;
    if(!SD_GetStatus(&status)) return false;

    if(!(status & SDIO_STATUS_CARD_INSERTED)){
        OSReport("[DX] SD card not inserted\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INITIALIZED))
    {
        OSReport("[DX] Could not initialize filesystem\n");
        return false;
    }

    sdhc = !!(status & SDIO_STATUS_CARD_SDHC);
    if(!SD_Enable4bitBus()){
        OSReport("[DX] Failed to enable 4-bit mode\n");
        return false;
    }

    if(!SD_SetClock(1)) return false;
    if(!SD_Select()) return false;

    if(!SD_SetCardBlockSize(SECTOR_SIZE)){
        SD_Deselect();
        return false;
    }

    if(!SD_EnableCard4BitBus){
        SD_Deselect();
        return false;
    }

    SD_Deselect();
    StorageDevice::currentDevice = new SDStorage();

    OSReport("[DX] Successfully initialized SD card\n");
    return true;
}

void InitSDCard()
{
    SDStorage::Init();
    return;
}

static BootHook bhSDCard(InitSDCard, LOW);