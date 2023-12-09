#pragma once

#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>
#include <kamek.hpp>
#include <main.hpp>

#define SDIO_HEAPSIZE				(5*1024)
 
#define PAGE_SIZE512				512

#define TMP_SECTOR_COUNT            8
#define SECTOR_SIZE                 512

#define	SDIOHCR_RESPONSE			0x10
#define SDIOHCR_HOSTCONTROL			0x28
#define	SDIOHCR_POWERCONTROL		0x29
#define	SDIOHCR_CLOCKCONTROL		0x2c
#define	SDIOHCR_TIMEOUTCONTROL		0x2e
#define	SDIOHCR_SOFTWARERESET		0x2f
 
#define SDIOHCR_HOSTCONTROL_4BIT	0x02

#define	SDIO_DEFAULT_TIMEOUT		0xe
 
#define IOCTL_SDIO_WRITEHCREG		0x01
#define IOCTL_SDIO_READHCREG		0x02
#define IOCTL_SDIO_READCREG			0x03
#define IOCTL_SDIO_RESETCARD		0x04
#define IOCTL_SDIO_WRITECREG		0x05
#define IOCTL_SDIO_SETCLK			0x06
#define IOCTL_SDIO_SENDCMD			0x07
#define IOCTL_SDIO_SETBUSWIDTH		0x08
#define IOCTL_SDIO_READMCREG		0x09
#define IOCTL_SDIO_WRITEMCREG		0x0A
#define IOCTL_SDIO_GETSTATUS		0x0B
#define IOCTL_SDIO_GETOCR			0x0C
#define IOCTL_SDIO_READDATA			0x0D
#define IOCTL_SDIO_WRITEDATA		0x0E
 
#define SDIOCMD_TYPE_BC				1
#define SDIOCMD_TYPE_BCR			2
#define SDIOCMD_TYPE_AC				3
#define SDIOCMD_TYPE_ADTC			4
 
#define SDIO_RESPONSE_NONE			0
#define SDIO_RESPONSE_R1			1
#define SDIO_RESPONSE_R1B			2
#define SDIO_RESPOSNE_R2			3
#define SDIO_RESPONSE_R3			4
#define SDIO_RESPONSE_R4			5
#define SDIO_RESPONSE_R5			6
#define SDIO_RESPONSE_R6			7
 
#define SDIO_CMD_GOIDLE				0x00
#define	SDIO_CMD_ALL_SENDCID		0x02
#define SDIO_CMD_SENDRCA			0x03
#define SDIO_CMD_SELECT				0x07
#define SDIO_CMD_DESELECT			0x07
#define	SDIO_CMD_SENDIFCOND			0x08
#define SDIO_CMD_SENDCSD			0x09
#define SDIO_CMD_SENDCID			0x0A
#define SDIO_CMD_SENDSTATUS			0x0D
#define SDIO_CMD_SETBLOCKLEN		0x10
#define SDIO_CMD_READBLOCK			0x11
#define SDIO_CMD_READMULTIBLOCK		0x12
#define SDIO_CMD_WRITEBLOCK			0x18
#define SDIO_CMD_WRITEMULTIBLOCK	0x19
#define SDIO_CMD_APPCMD				0x37
 
#define SDIO_ACMD_SETBUSWIDTH		0x06
#define SDIO_ACMD_SENDSCR			0x33
#define	SDIO_ACMD_SENDOPCOND		0x29

#define	SDIO_STATUS_CARD_INSERTED		0x1
#define	SDIO_STATUS_CARD_INITIALIZED	0x10000
#define SDIO_STATUS_CARD_SDHC			0x100000

#define READ_BL_LEN					((u8)(__sd0_csd[5]&0x0f))
#define WRITE_BL_LEN				((u8)(((__sd0_csd[12]&0x03)<<2)|((__sd0_csd[13]>>6)&0x03)))

#define ATTRIBUTE_ALIGN(x) __attribute__((aligned(x)))

#define STACK_ALIGN(type, name, cnt, alignment)		u8 _al__##name[((sizeof(type)*(cnt)) + (alignment) + (((sizeof(type)*(cnt))%(alignment)) > 0 ? ((alignment) - ((sizeof(type)*(cnt))%(alignment))) : 0))]; \
													type *name = (type*)(((u32)(_al__##name)) + ((alignment) - (((u32)(_al__##name))&((alignment)-1))))

struct _sdiorequest
{
	u32 cmd;
	u32 cmd_type;
	u32 rsp_type;
	u32 arg;
	u32 blk_cnt;
	u32 blk_size;
	void *buffer;
	u32 isdma;
	u32 pad0;
};
 
struct _sdioresponse
{
	u32 rsp_fields[3];
	u32 acmd12_response;
};

static s32 sdfd = -1;
static u16 rca = 0;
static bool sdhc;

bool SD_Reset();
bool SD_Reset(s32 fd);
bool SD_GetStatus(u32 * status);
bool SD_ReadHcr(u8 reg, u8 size, u32 * val);
bool SD_WriteHcr(u8 reg, u8 size, u32 val);
bool SD_SetClock(u32 clock);
bool SD_SendCommand(u32 command, u32 commandType, u32 responseType, u32 arg, u32 blockCount, u32 blockSize, void * buffer, u32 * response);
bool SD_Enable4bitBus();
bool SD_Select();
bool SD_Deselect();
bool SD_SetCardBlockSize(u32 blockSize);
bool SD_EnableCard4BitBus();
bool SD_TransferAligned(bool isWrite, u32 sector, u32 sectorCount, void * buffer);
bool SD_Transfer(bool isWrite, u32 sector, u32 sectorCount, void * buffer);
u32 SD_SectorSize();
bool SD_Read(u32 sector, u32 sectorCount, void * buffer);
bool SD_Write(u32 sector, u32 sectorCount, const void * buffer);
bool SD_Erase();
u32 SD_GetMessageId();