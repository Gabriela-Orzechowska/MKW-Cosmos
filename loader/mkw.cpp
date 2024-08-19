#include "core/rvl/ipc/ipc.hpp"
#include "functions.hpp"
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
	(sprintf_t) 0x80011A2C,
    (RKSystem*) 0x802A4080,
	(ARCInitHandle_t) 0x80124500,
	(ARCOpen_t) 0x801245a0,
	(SZS_Decode_t) 0x80218c2c,
	(NANDPrivateOpen_t) 0x8019C88C,
	(NANDClose_t) 0x8019CA80, 
	(NANDRead_t) 0x8019B7A4,
	(NANDGetLength_t) 0x8019BF4C,
    0x80242698,
    0x8000A3F4,
    }
};
loaderFunctionsEx functions_e = {
	{(OSReport_t) 0x801A2530,
	(OSFatal_t) 0x801A4E24,
	(sprintf_t) 0x80010ECC,
    (RKSystem*) 0x8029fd00,
	(ARCInitHandle_t) 0x80124460,
	(ARCOpen_t) 0x80124500,
	(SZS_Decode_t) 0x80218b8c,
	(NANDPrivateOpen_t) 0x8019c7ec,
	(NANDClose_t) 0x8019c9e0, 
	(NANDRead_t) 0x8019b704,
	(NANDGetLength_t) 0x8019beac,
    0x802417dc,
    0x8000A3B4,
}};

loaderFunctionsEx functions_j = {
	{(OSReport_t) 0x801A24F0,
	(OSFatal_t) 0x801A4DE4,
	(sprintf_t) 0x80011950,
    (RKSystem*) 0x802a3a00,
	(ARCInitHandle_t) 0x80124420,
	(ARCOpen_t) 0x801244c0,
	(SZS_Decode_t) 0x80218b4c,
	(NANDPrivateOpen_t) 0x8019c7ac,
	(NANDClose_t) 0x8019c9a0, 
	(NANDRead_t) 0x8019b6c4,
	(NANDGetLength_t) 0x8019be6c,
    0x802425b8,
    0x8000A350,
}};
loaderFunctionsEx functions_k = {
	{(OSReport_t) 0x801A292C,
	(OSFatal_t) 0x801A5220,
	(sprintf_t) 0x80011A94,
    (RKSystem*) 0x80292080,
	(ARCInitHandle_t) 0x80124578,
	(ARCOpen_t) 0x80124618,
	(SZS_Decode_t) 0x80218fa0,
	(NANDPrivateOpen_t) 0x8019cbe8,
	(NANDClose_t) 0x8019cddc, 
	(NANDRead_t) 0x8019bb00,
	(NANDGetLength_t) 0x8019c2a8,
    0x80242a0c,
    0x8000A4FC,
}
};

void loadIntoMKW();

inline void cacheInvalidateAddress(u32 address);
static const char* Console_Write(const char* str);
static loaderFunctions* funcs = nullptr;
static DVDFunctions* dvdFuncs = nullptr;
static IOSFunctions* iosFuncs = nullptr;

int PerformExploit() {
    u8 region = *(u8 *)(0x80000003);
    
	// choose functions
    int *heap = NULL;
	switch (region)
	{
		case 'P': funcs = &functions_p.base; break;
		case 'E': funcs = &functions_e.base; break;
		case 'J': funcs = &functions_j.base; break;
		case 'K': funcs = &functions_k.base; break;
	}
    dvdFuncs = dvdFunctionsSets[GetRegionIndex()];
    iosFuncs = iosFunctionsSets[GetRegionIndex()];

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
    Console_Print("[IOS] Opening FS\n");

    return iosFuncs->Open("/dev/fs", 0);
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
	// choose functions
    int *heap = NULL;

	char path[64];
	funcs->sprintf(path, "%c.szs", *(u8*)0x80000003);
	loadKamekBinaryFromDisc(funcs, "/Binaries/Code.cscd", path);
    
    return;
};
