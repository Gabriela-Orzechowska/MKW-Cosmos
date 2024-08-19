#include <kamekLoader.hpp>
#include <console.hpp>


struct KBHeader {
    u32 magic1;
    u16 magic2;
    u16 version;
    u32 bssSize;
    u32 codeSize;
    u32 ctorStart;
    u32 ctorEnd;
    u32 _pad[2];
};


#define kAddr32 1
#define kAddr16Lo 4
#define kAddr16Hi 5
#define kAddr16Ha 6
#define kRel24 10
#define kWrite32 32
#define kWrite16 33
#define kWrite8 34
#define kCondWritePointer 35
#define kCondWrite32 36
#define kCondWrite16 37
#define kCondWrite8 38
#define kBranch 64
#define kBranchLink 65


void kamekError(const loaderFunctions *funcs, const char *str) {
    u32 fg = 0xFFFFFFFF, bg = 0;
    funcs->OSFatal(&fg, &bg, str);
}


static inline u32 resolveAddress(u32 text, u32 address) {
    if (address & 0x80000000)
        return address;
    else
        return text + address;
}


#define kCommandHandler(name) \
	static inline const u8 *kHandle##name(const u8 *input, u32 text, u32 address)
#define kDispatchCommand(name) \
	case k##name: input = kHandle##name(input, text, address); break

kCommandHandler(Addr32) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    *(u32 *)address = target;
    return input + 4;
}
kCommandHandler(Addr16Lo) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    *(u16 *)address = target & 0xFFFF;
    return input + 4;
}
kCommandHandler(Addr16Hi) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    *(u16 *)address = target >> 16;
    return input + 4;
}
kCommandHandler(Addr16Ha) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    *(u16 *)address = target >> 16;
    if (target & 0x8000)
        *(u16 *)address += 1;
    return input + 4;
}
kCommandHandler(Rel24) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    u32 delta = target - address;
    *(u32 *)address &= 0xFC000003;
    *(u32 *)address |= (delta & 0x3FFFFFC);
    return input + 4;
}
kCommandHandler(Write32) {
    u32 value = *(const u32 *)input;
    *(u32 *)address = value;
    return input + 4;
}
kCommandHandler(Write16) {
    u32 value = *(const u32 *)input;
    *(u16 *)address = value & 0xFFFF;
    return input + 4;
}
kCommandHandler(Write8) {
    u32 value = *(const u32 *)input;
    *(u8 *)address = value & 0xFF;
    return input + 4;
}
kCommandHandler(CondWritePointer) {
    u32 target = resolveAddress(text, *(const u32 *)input);
    u32 original = ((const u32 *)input)[1];
    if (*(u32 *)address == original)
        *(u32 *)address = target;
    return input + 8;
}
kCommandHandler(CondWrite32) {
    u32 value = *(const u32 *)input;
    u32 original = ((const u32 *)input)[1];
    if (*(u32 *)address == original)
        *(u32 *)address = value;
    return input + 8;
}
kCommandHandler(CondWrite16) {
    u32 value = *(const u32 *)input;
    u32 original = ((const u32 *)input)[1];
    if (*(u16 *)address == (original & 0xFFFF))
        *(u16 *)address = value & 0xFFFF;
    return input + 8;
}
kCommandHandler(CondWrite8) {
    u32 value = *(const u32 *)input;
    u32 original = ((const u32 *)input)[1];
    if (*(u8 *)address == (original & 0xFF))
        *(u8 *)address = value & 0xFF;
    return input + 8;
}
kCommandHandler(Branch) {
    *(u32 *)address = 0x48000000;
    return kHandleRel24(input, text, address);
}
kCommandHandler(BranchLink) {
    *(u32 *)address = 0x48000001;
    return kHandleRel24(input, text, address);
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

void loadKamekBinary(loaderFunctions *funcs, const void *binary, u32 binaryLength, bool isDol)
{
    static u32 text = 0;
    const KBHeader *header = (const KBHeader *)binary;
    if (header->magic1 != 'Kame' || header->magic2 != 'k\0')
        kamekError(funcs, "FATAL ERROR: Corrupted file, please check your game's Kamek files");
    if (header->version != 2) {
        char err[512];
        funcs->sprintf(err, "FATAL ERROR: Incompatible file (version %d), please upgrade your Kamek Loader", header->version);
        kamekError(funcs, err);
    }

    funcs->OSReport("header: bssSize=%u, codeSize=%u, ctors=%u-%u\n",
        header->bssSize, header->codeSize, header->ctorStart, header->ctorEnd);

    u32 textSize = header->codeSize + header->bssSize;

    EGG::ExpHeap *heap = funcs->rkSystem->EGGSystem;
    if(isDol) text = (u32)heap->alloc(textSize, 0x20);


    if (!text)
        kamekError(funcs, "FATAL ERROR: Out of code memory");

    const u8 *input = ((const u8 *)binary) + sizeof(KBHeader);
    const u8 *inputEnd = ((const u8 *)binary) + binaryLength;
    u8 *output = (u8 *)text;

    *(u32 *)0x80003FFC = (u32)text;
    *(u32 *)0x80003FF8 = (u32)textSize;
    *(u32 *)0x80003FF4 = (u32)header->codeSize;
    *(u32 *)0x80003FF0 = (u32)header->ctorStart;
    *(u32 *)0x80003FEC = (u32)Console_Print;
    *(u32 *)0x80003FE8 = (u32)Console_Destroy;
    *(u32 *)0x80003FE4 = (u32)Console_Clear;


    if(isDol)
    {
        // Create text + bss sections
        for (u32 i = 0; i < header->codeSize; i++) {
            *output = *(input++);
            cacheInvalidateAddress((u32)(output++));
        }
        for (u32 i = 0; i < header->bssSize; i++) {
            *output = 0;
            cacheInvalidateAddress((u32)(output++));
        }
        Console_Print("[CSE] Applying dol patches\n");

    }
    else Console_Print("[CSE] Applying rel patches\n");
    while (input < inputEnd) {
        u32 cmdHeader = *((u32 *)input);
        input += 4;

        u8 cmd = cmdHeader >> 24;
        u32 address = cmdHeader & 0xFFFFFF;
        if (address == 0xFFFFFE) {
            // Absolute address
            address = *((u32 *)input);
            if(address < 0x80510238 && !isDol) continue;
            else if(address >= 0x80510238 && isDol) continue;
            input += 4;
        }
        else {
            // Relative address
            if(!isDol) continue;
            address += text;
        }

        switch (cmd) {
            kDispatchCommand(Addr32);
            kDispatchCommand(Addr16Lo);
            kDispatchCommand(Addr16Hi);
            kDispatchCommand(Addr16Ha);
            kDispatchCommand(Rel24);
            kDispatchCommand(Write32);
            kDispatchCommand(Write16);
            kDispatchCommand(Write8);
            kDispatchCommand(CondWritePointer);
            kDispatchCommand(CondWrite32);
            kDispatchCommand(CondWrite16);
            kDispatchCommand(CondWrite8);
            kDispatchCommand(Branch);
            kDispatchCommand(BranchLink);
        }

        cacheInvalidateAddress(address);
    }
    __sync();
    __isync();

    typedef void (*Func)(void);
    if(!isDol)
    {
        for (Func *f = (Func *)(text + header->ctorStart); f < (Func *)(text + header->ctorEnd); f++) {
            (*f)();
        }
    }
}

static void* codeBuf = nullptr;
static DVDFunctions* dvdFuncs = nullptr;
void loadKamekBinaryFromDisc(loaderFunctions *funcs, const char *path, const char* codePath)
{
    if(((u32)codeBuf & 0xFF000000) != 0x80000000) codeBuf = nullptr;
    dvdFuncs = dvdFunctionsSets[GetRegionIndex()];
    static u32 fileLength = 0;
    funcs->OSReport("{Kamek by Treeki}\nLoading Kamek binary '%s'...\n", path);
    bool isDol = false;

    EGG::ExpHeap *heap = funcs->rkSystem->EGGSystem;
    if(codeBuf == nullptr){

        Console_Print("[CSE] Loading payload\n");
        int entrynum = dvdFuncs->ConvertPathToEntrynum(path);

        DVDFileInfo fileInfo;
        if (entrynum >= 0 && !dvdFuncs->FastOpen(entrynum, &fileInfo))
            kamekError(funcs, "FATAL ERROR: Failed to open file!");

        u32 length = fileInfo.length;

        u32 dvdVersion = 0;
        u32 nandVersion = 0;
        bool usesNand = false;

        if(entrynum >= 0 && !dvdFuncs->ReadPrio(&fileInfo, (void*)bufferPointer, 0x20, 0, 2)){
            kamekError(funcs, "Failed to load file from dics!");
        }
        dvdVersion = *(u32*) (bufferPointer + 0x10);

        NANDFileInfo nandInfo;

        if(funcs->NANDPrivateOpen("/title/00010001/43534D53/Code.arc", &nandInfo, NAND_ACCESS_READ) == NAND_RESULT_OK){
            u32 nandFileLength = 0;
            funcs->OSReport("Found binary on NAND\n");
            if(funcs->NANDGetLength(&nandInfo, &nandFileLength) == NAND_RESULT_OK){
                if(funcs->NANDRead(&nandInfo, (void*)bufferPointer, 0x20) != 0){
                    nandVersion = *(u32*) (bufferPointer + 0x10);
                    if(nandVersion >= dvdVersion && dvdVersion != devVersion){
                        if(funcs->NANDRead(&nandInfo, (void*)(bufferPointer + 0x20), (nandFileLength - 0x20)) != 0){
                            usesNand = true;
                            funcs->OSReport("Loading code from NAND...\n");
                            Console_Print("from NAND\n");
                        }
                    }
                    else{
                        if(dvdVersion == devVersion) {
                            funcs->OSReport("Using disc file, development version found\n");
                            Console_Print("from disc (dev version)\n");
                        }
                        else {
                            funcs->OSReport("Using disc file, newer version found\n");
                            Console_Print("from disc\n");
                        }
                        
                    }
                }
            }
            funcs->NANDClose(&nandInfo);
        }

        if(!usesNand)
        {
            if (entrynum < 0) {
                char err[512];
                funcs->sprintf(err, "FATAL ERROR: Failed to locate file on the disc: %s", path);
                kamekError(funcs, err);
            }
            funcs->OSReport("Loading code from disc...\n");
            Console_Print("from disc\n");
            funcs->DVDReadPrio(&fileInfo, (void*)bufferPointer, length, 0, 2);
            funcs->DVDClose(&fileInfo);
        }


        ARCHandle handle;
        funcs->ARCInitHandle((void*)bufferPointer, &handle);

        ARCFileInfo info;
        funcs->ARCOpen(&handle, codePath, &info);

        YAZHeader* header = (YAZHeader*) (bufferPointer + info.startOffset);

        u32 roundedLength = (header->fileSize + 0x1F) & ~0x1F;
        codeBuf = heap->alloc(roundedLength, -0x20);

        if (!codeBuf) {
            Console_Print("[CSE] Failed to allocate space!\n");
            for(;;);
        }

        funcs->SZS_Decode((void*)(bufferPointer +info.startOffset), codeBuf);

        isDol = true;
        fileLength = header->fileSize;

        for(u32 i = bufferPointer; i < (bufferPointer + length); i++){
            *(u8*)i = 0;
        }
    }
    
    loadKamekBinary(funcs, codeBuf, fileLength, isDol);
    if(!isDol) heap->free(codeBuf);
}
