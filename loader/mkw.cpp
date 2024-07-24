#include "core/rvl/ipc/ipc.hpp"
#include <core/rvl/os/OS.hpp>
#include <kamekLoader.hpp>
#include <console.hpp>

void loadIntoMKW();

struct loaderFunctionsEx {
	loaderFunctions base;
};

enum SC {
    IOS_SetUid = 0x2B,
    IOS_InvalidateDCache = 0x3F,
    IOS_FlushDCache = 0x40,
};

loaderFunctionsEx functions_p = {
	{(OSReport_t) 0x801A25D0,
	(OSFatal_t) 0x801A4EC4,
	(DVDConvertPathToEntrynum_t) 0x8015DF4C,
	(DVDFastOpen_t) 0x8015E254,
	(DVDReadPrio_t) 0x8015E834,
	(DVDClose_t) 0x8015E568,
	(sprintf_t) 0x80011A2C,
    (RKSystem*) 0x802A4080,
	(ARCInitHandle_t) 0x80124500,
	(ARCOpen_t) 0x801245a0,
	(SZS_Decode_t) 0x80218c2c,
	(NANDPrivateOpen_t) 0x8019C88C,
	(NANDClose_t) 0x8019CA80, 
	(NANDRead_t) 0x8019B7A4,
	(NANDGetLength_t) 0x8019BF4C,
    (IOS_Open_t) 0x801938f8,
    (IOS_Ioctlv_t) 0x801945e0,
    (IOS_Write_t) 0x80193e88,
    (IOS_Close_t) 0x80193ad8,
    0x80242698,
    0x8000A3F4,
    0x802a26f0,
    (VIGetNextFrameBuffer_t) 0x801bab24,
    (DirectPrint_ChangeXfb_t) 0x80021e30,
    (DirectPrint_DrawString_t) 0x80021f80,
    (DirectPrint_StoreCache_t) 0x80021e70,
    }
};
loaderFunctionsEx functions_e = {
	{(OSReport_t) 0x801A2530,
	(OSFatal_t) 0x801A4E24,
	(DVDConvertPathToEntrynum_t) 0x8015DEAC,
	(DVDFastOpen_t) 0x8015E1B4,
	(DVDReadPrio_t) 0x8015E794,
	(DVDClose_t) 0x8015E4C8,
	(sprintf_t) 0x80010ECC,
    (RKSystem*) 0x8029fd00,
	(ARCInitHandle_t) 0x80124460,
	(ARCOpen_t) 0x80124500,
	(SZS_Decode_t) 0x80218b8c,
	(NANDPrivateOpen_t) 0x8019c7ec,
	(NANDClose_t) 0x8019c9e0, 
	(NANDRead_t) 0x8019b704,
	(NANDGetLength_t) 0x8019beac,
    (IOS_Open_t) 0x80193858,
    (IOS_Ioctlv_t) 0x80194540,
    (IOS_Write_t) 0x80193de8,
    (IOS_Close_t) 0x80193a38,
    0x802417dc,
    0x8000A3B4,
    0x8029e3b0,
    (VIGetNextFrameBuffer_t) 0x801baa84,
    (DirectPrint_ChangeXfb_t) 0x80021d90,
    (DirectPrint_DrawString_t) 0x80021ee0,
    (DirectPrint_StoreCache_t) 0x80021dd0,
}};

loaderFunctionsEx functions_j = {
	{(OSReport_t) 0x801A24F0,
	(OSFatal_t) 0x801A4DE4,
	(DVDConvertPathToEntrynum_t) 0x8015DE6C,
	(DVDFastOpen_t) 0x8015E174,
	(DVDReadPrio_t) 0x8015E754,
	(DVDClose_t) 0x8015E488,
	(sprintf_t) 0x80011950,
    (RKSystem*) 0x802a3a00,
	(ARCInitHandle_t) 0x80124420,
	(ARCOpen_t) 0x801244c0,
	(SZS_Decode_t) 0x80218b4c,
	(NANDPrivateOpen_t) 0x8019c7ac,
	(NANDClose_t) 0x8019c9a0, 
	(NANDRead_t) 0x8019b6c4,
	(NANDGetLength_t) 0x8019be6c,
    (IOS_Open_t) 0x80193818,
    (IOS_Ioctlv_t) 0x80194500,
    (IOS_Write_t) 0x80193da8,
    (IOS_Close_t) 0x801939f8,
    0x802425b8,
    0x8000A350,
    0x802a2090,
    (VIGetNextFrameBuffer_t) 0x801baa44,
    (DirectPrint_ChangeXfb_t) 0x80021d50,
    (DirectPrint_DrawString_t) 0x80021ea0,
    (DirectPrint_StoreCache_t) 0x80021d90,
}};
loaderFunctionsEx functions_k = {
	{(OSReport_t) 0x801A292C,
	(OSFatal_t) 0x801A5220,
	(DVDConvertPathToEntrynum_t) 0x8015DFC4,
	(DVDFastOpen_t) 0x8015E2CC,
	(DVDReadPrio_t) 0x8015E8AC,
	(DVDClose_t) 0x8015E5E0,
	(sprintf_t) 0x80011A94,
    (RKSystem*) 0x80292080,
	(ARCInitHandle_t) 0x80124578,
	(ARCOpen_t) 0x80124618,
	(SZS_Decode_t) 0x80218fa0,
	(NANDPrivateOpen_t) 0x8019cbe8,
	(NANDClose_t) 0x8019cddc, 
	(NANDRead_t) 0x8019bb00,
	(NANDGetLength_t) 0x8019c2a8,
    (IOS_Open_t) 0x80193c54,
    (IOS_Ioctlv_t) 0x8019493c,
    (IOS_Write_t) 0x801941e4,
    (IOS_Close_t) 0x80193e34,
    0x80242a0c,
    0x8000A4FC,
    0x802906f0,
    (VIGetNextFrameBuffer_t) 0x801bae80,
    (DirectPrint_ChangeXfb_t) 0x80021e90,
    (DirectPrint_DrawString_t) 0x80021fe0,
    (DirectPrint_StoreCache_t) 0x80021ed0,
}
};

void loadIntoMKW();

inline void cacheInvalidateAddress(u32 address);
static const char* Console_Write(const char* str);

int PerformExploit() {
    u8 region = *(u8 *)(0x80000003);
    
	// choose functions
	loaderFunctions *funcs = NULL;
    int *heap = NULL;
	switch (region)
	{
		case 'P': funcs = &functions_p.base; break;
		case 'E': funcs = &functions_e.base; break;
		case 'J': funcs = &functions_j.base; break;
		case 'K': funcs = &functions_k.base; break;
	}

    //Create Branches
    u32 offset = ((u32)&loadIntoMKW)-funcs->dolHookAddress;
    u32 command = 0x48000000 | (offset & 0x03FFFFFF);
    *((u32*)funcs->dolHookAddress) = command;
    cacheInvalidateAddress(funcs->dolHookAddress);
    
    offset = ((u32)&loadIntoMKW)-funcs->relHookAddress;
    command = 0x48000000 | (offset & 0x03FFFFFF);
    *((u32*)funcs->relHookAddress) = command;
    cacheInvalidateAddress(funcs->relHookAddress);

    Console_Init(funcs);
    Console_Print("  _____                      \n / ___/__  ___ __ _  ___  ___\n/ /__/ _ \\(_-</  ' \\/ _ \\(_-<\n\\___/\\___/___/_/_/_/\\___/___/\n");
    //Console_Print("Cosmos Loader v1.2\n");
    Console_Print("Opening /dev/fs\n");

    return funcs->IOS_Open("/dev/fs", 0);
}

void unknownVersion()
{
	// can't do much here!
	// we can't output a message on screen without a valid OSFatal addr
	for (;;);
}

inline void cacheInvalidateAddress(u32 address) {
    register u32 addressRegister = address;
    asm{
        ASM(
        dcbst 0, addressRegister;
        sync;
        icbi 0, addressRegister;
        )
    }
}
void loadIntoMKW()
{
    u8 region = *(u8 *)(0x80000003);
    
	// choose functions
	loaderFunctions *funcs = NULL;
    int *heap = NULL;
	switch (region)
	{
		case 'P': funcs = &functions_p.base; break;
		case 'E': funcs = &functions_e.base; break;
		case 'J': funcs = &functions_j.base; break;
		case 'K': funcs = &functions_k.base; break;
	}

	char path[64];
	funcs->sprintf(path, "%c.szs", region);
	loadKamekBinaryFromDisc(funcs, "/Binaries/Code.cscd", path);
    
    return;
};
