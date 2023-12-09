#include <Storage/disc/SD.hpp>

#define ALIGN_BUFFER  __attribute__ ((aligned(0x20)))

static u8 tmpBuffer[TMP_SECTOR_COUNT * SECTOR_SIZE] ALIGN_BUFFER;

static u32 out ALIGN_BUFFER;

bool SD_Reset(s32 fd)
{
    sdfd = fd;
    return SD_Reset();
}

bool SD_Reset()
{
    if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_RESETCARD, NULL, 0, &out, sizeof(out)) < 0) return false;

    rca = out >> 16;
    return true;
}

bool SD_GetStatus(u32 * status)
{
    if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_GETSTATUS, NULL, 0, &out, sizeof(out)) < 0) return false;
    *status = out;
    return true;
}

bool SD_ReadHcr(u8 reg, u8 size, u32 * val)
{
    u32 in[0x6] ALIGN_BUFFER;
    in[0] = reg;
    in[1] = 0;
    in[2] = 0;
    in[3] = size;
    in[4] = 0;
    in[5] = 0;

    if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_READHCREG, in, sizeof(in), &out, sizeof(out)) < 0) return false;
    *val = out;
    return true;
}

bool SD_WriteHcr(u8 reg, u8 size, u32 val)
{
    u32 in[0x6] ALIGN_BUFFER;
    in[0] = reg;
    in[1] = 0;
    in[2] = 0;
    in[3] = size;
    in[4] = val;
    in[5] = 0;

    if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_WRITEHCREG, in, sizeof(in), NULL, 0) < 0) return false;
    return true;
}

bool SD_SetClock(u32 clock)
{
    u32 in ALIGN_BUFFER;
    in = clock;

    if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_SETCLK, &in, sizeof(in), NULL, 0) < 0) return false;   
    return true;
}

bool SD_SendCommand(u32 command, u32 commandType, u32 responseType, u32 arg, u32 blockCount, u32 blockSize, void * buffer, u32 * response)
{
    _sdiorequest request ALIGN_BUFFER;
    request.cmd = command;
    request.cmd_type = commandType;
    request.rsp_type = responseType;
    request.arg = arg;
    request.blk_cnt = blockCount;
    request.blk_size = blockSize;
    request.buffer = buffer;
    request.isdma = !!buffer;
    request.pad0 = 0;

    u32 tmpOut[0x4] ALIGN_BUFFER;

    if(buffer || sdhc)
    {
        IOS::IOCtlvRequest vec[0x3] ALIGN_BUFFER;
        vec[0].address = &request;
        vec[0].size = sizeof(request);
        vec[1].address = buffer;
        vec[1].size = blockCount * blockSize;
        vec[2].address = tmpOut;
        vec[2].size = sizeof(tmpOut);

        if(IOS::IOCtlv(sdfd, (IOS::IOCtlType) IOCTL_SDIO_SENDCMD, 2, 1, vec) < 0){
            return false;
        }
    }
    else
    {
        if(IOS::IOCtl(sdfd, (IOS::IOCtlType) IOCTL_SDIO_SENDCMD, &request, sizeof(request), &tmpOut, sizeof(out)) < 0)
        {
            return false;
        }
    }

    if(response) *response = tmpOut[0];
    return true;
}

bool SD_Enable4bitBus()
{
    u32 val;
    if(!SD_ReadHcr(SDIOHCR_HOSTCONTROL, sizeof(u8), &val))
    {
        return false;
    }
    val |= SDIOHCR_HOSTCONTROL_4BIT;

    return SD_WriteHcr(SDIOHCR_HOSTCONTROL, sizeof(u8), val);
}

bool SD_Select()
{
    return SD_SendCommand(SDIO_CMD_SELECT, 3, SDIO_RESPONSE_R1B, rca << 16, 0,0,NULL,NULL);
}
bool SD_Deselect()
{
    return SD_SendCommand(SDIO_CMD_SELECT, 3, SDIO_RESPONSE_R1B, 0, 0,0,NULL,NULL);
}
bool SD_SetCardBlockSize(u32 blockSize)
{
    return SD_SendCommand(SDIO_CMD_SETBLOCKLEN, 3, SDIO_RESPONSE_R1, blockSize, 0, 0, NULL, NULL);
}

bool SD_EnableCard4BitBus()
{
    if(!SD_SendCommand(SDIO_CMD_APPCMD, 3, SDIO_RESPONSE_R1, rca << 16, 0, 0, NULL, NULL)) return false;
    return SD_SendCommand(SDIO_ACMD_SETBUSWIDTH, 3, SDIO_RESPONSE_R1, 0x2, 0, 0, NULL, NULL);
}

bool SD_TransferAligned(bool isWrite, u32 sector, u32 sectorCount, void * buffer)
{
    u32 command = isWrite ? SDIO_CMD_WRITEMULTIBLOCK : SDIO_CMD_READMULTIBLOCK;
    u32 firstBlock = sdhc ? sector : sector * SECTOR_SIZE;
    return SD_SendCommand(command, 3, SDIO_RESPONSE_R1, firstBlock, sectorCount, SECTOR_SIZE, buffer, NULL);
}

bool SD_Transfer(bool isWrite, u32 sector, u32 sectorCount, void * buffer)
{
    if(!buffer) return false;

    if(!SD_Select()) return false;

    if((u32)buffer & 0x1F) // Check if aligned
    {
        while(sectorCount > 0)
        {
            u32 chunkSectorCount = sectorCount < TMP_SECTOR_COUNT ? sectorCount : TMP_SECTOR_COUNT;
            if(isWrite)
            {
                memcpy(tmpBuffer, buffer, chunkSectorCount * SECTOR_SIZE);
            }
            if(!SD_TransferAligned(isWrite, sector,chunkSectorCount, tmpBuffer)){
                SD_Deselect();
                return false;
            }
            if(!isWrite){
                memcpy(buffer, tmpBuffer, chunkSectorCount * SECTOR_SIZE);
            }

            sector += chunkSectorCount;
            sectorCount -= chunkSectorCount;
            buffer = (void *) ((u32)buffer + (chunkSectorCount * SECTOR_SIZE));
        }
    }
    else{
        if(!SD_TransferAligned(isWrite, sector, sectorCount, buffer)){
            SD_Deselect();
            return false;
        }
    }
    SD_Deselect();
    return true;
}

u32 SD_SectorSize(){
    return SECTOR_SIZE;
}

bool SD_Read(u32 sector, u32 sectorCount, void * buffer)
{
    return SD_Transfer(false, sector, sectorCount, buffer);
}
bool SD_Write(u32 sector, u32 sectorCount, const void * buffer)
{
    return SD_Transfer(true, sector, sectorCount, (void *)buffer);
}

bool SD_Erase(){return true;}

u32 SD_GetMessageId()
{
    return 10154;
}
