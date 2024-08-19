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

#define syscall(id) 0xE6000010 | (u8)id << 5

static u32 code[] = {
        /* 0x00 */ 0xEA000000, // b       0x8
    /* 0x04 */ 0x00000000, // MESSAGE_VALUE
    // Set PPC UID to root
    /* 0x08 */ 0xE3A0000F, // mov     r0, #15
    /* 0x0C */ 0xE3A01000, // mov     r1, #0
    /* 0x10 */ syscall(IOS_SetUid),
    // Send response to PPC
    /* 0x14 */ 0xE24F0018, // adr     r0, MESSAGE_VALUE
    /* 0x18 */ 0xE3A01001, // mov     r1, #1
    /* 0x1C */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x20 */ 0xE3A01004, // mov     r1, #4
    /* 0x24 */ syscall(IOS_FlushDCache),
    // Wait for response back from PPC
    // loop_start:
    /* 0x28 */ 0xE24F002C, // adr     r0, MESSAGE_VALUE
    /* 0x2C */ 0xE5902000, // ldr     r2, [r0]
    /* 0x30 */ 0xE3520002, // cmp     r2, #2
    /* 0x34 */ 0x0A000001, // beq     loop_break
    /* 0x38 */ syscall(IOS_InvalidateDCache),
    /* 0x3C */ 0xEAFFFFF9, // b       loop_start
    // loop_break:
    // Reset PPC UID back to 15
    /* 0x40 */ 0xE3A0000F, // mov     r0, #15
    /* 0x44 */ 0xE3A0100F, // mov     r1, #15
    /* 0x48 */ syscall(IOS_SetUid),
    // Send response to PPC
    /* 0x4C */ 0xE24F0050, // adr     r0, MESSAGE_VALUE
    /* 0x50 */ 0xE3A01003, // mov     r1, #3
    /* 0x54 */ 0xE5801000, // str     r1, [r0]
    // Flush the response to main memory
    /* 0x58 */ 0xE3A01004, // mov     r1, #4
    /* 0x5C */ syscall(IOS_FlushDCache),
    /* 0x60 */ 0xE12FFF1E, // bx      lr
};


void loadIntoMKW();

inline void cacheInvalidateAddress(u32 address);
static const char* Console_Write(const char* str);
void VerifyDol(loaderFunctions* funcs);
static loaderFunctions* funcs = nullptr;
static DVDFunctions* dvdFuncs = nullptr;
static IOSFunctions* iosFuncs = nullptr;

int PerformExploit() {
    register u32 branchAddress;
    asm{ASM(mflr branchAddress;)}
    *((u32*)(branchAddress - 4)) = 0x48029ca5;
    cacheInvalidateAddress(branchAddress - 4);


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

    VerifyDol(funcs);
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

// Nothing for now, maybe i will come back to it and figure out whats wrong with this
/*
    s32 isDol = funcs->IOS_Open("/dev/dolphin", 0);
    if(isDol >= 0) {
        funcs->IOS_Close(isDol);
        return;
    }

    u32* tempBuffer = (u32*) 0x80700000;
    u32* memStart = (u32*) 0x80000000;    

    s32 sha = funcs->IOS_Open("/dev/sha", 0);
    if(sha < 0) {
        funcs->IOS_Close(sha);
        return;
    }

    for(int i = 0; i < 0x40; i++) {
        tempBuffer[i] = memStart[i];
        memStart[i] = 0x0;
    }

    funcs->IOS_Write(-1, code, sizeof(code));


    memStart[0] = 0x4903468D;
    memStart[1] = 0x49034788;
    memStart[2] = 0x49036209;
    memStart[3] = 0x47080000;
    memStart[4] = 0x10100000;
    memStart[5] = (u32)&code - 0x80000000;
    memStart[6] = 0xFFFF0014;
    
    IOS::IOCtlvRequest vec[4] __attribute((aligned(0x20)));
    vec[0].address = nullptr;
    vec[0].size = 0;
    vec[1].address = (void*) 0xFFFE0028;
    vec[1].size = 0;
    vec[2].address = (void*) 0x80000000;
    vec[2].size = 0x40;

    s32 ret = funcs->IOS_IOCtlv(sha, 0, 1, 2, vec);
    for(int i = 0; i < 0x40; i++){
       memStart[i] = tempBuffer[i];
    }
    return;
    funcs->IOS_Close(sha);
        *((u32*)0x80003FE0) = ret;
    ret = funcs->IOS_Open("/dev/fs", 0);

    for(int i = 0; i < 0x40; i++){
       memStart[i] = tempBuffer[i];
    }
    return;
*/
}

void unknownVersion()
{
	// can't do much here!
	// we can't output a message on screen without a valid OSFatal addr
	for (;;);
}

static u32 GetMessage(u32 index) {
    register u32 val = 0xC0000000 | (u32)(&code[index]);
    register u32 ret = 0;
    asm {
        ASM(
                lwz ret, 0x0 (val);
                sync;
           )
    }
    return ret;
}

static u32 SetMessage(u32 index, u32 message) {
    register u32 val = 0xC0000000 | (u32)(&code[index]);
    register u32 mess = message;
    asm {
        ASM (
            stw mess, 0x0 (val);
            eieio;
        )
    }
}
void VerifyDol(loaderFunctions* funcs){
    NETSha1Context context;
    u32 output[5];
    funcs->NETSHA1Init(&context);
    funcs->NETSHA1Update(&context, (void*)0x800072c0, funcs->dolSize);
    funcs->NETSHA1GetDigest(&context, (void*)&output);
    for(int i = 0; i < 5; i++){
        if(output[i] != funcs->dolHash[i])
        {
            u32 black = 0; u32 white = -1U;
            funcs->OSFatal(&black, &white, "main.dol has been modified!");
        }
    }
    return;
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
