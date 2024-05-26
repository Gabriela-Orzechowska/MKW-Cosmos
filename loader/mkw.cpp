#include <kamekLoader.hpp>

void loadIntoMKW();

struct loaderFunctionsEx {
	loaderFunctions base;
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
	(NANDGetLength_t) 0x8019BF4C }
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
	(NANDPrivateOpen_t) 0x8019C88C,
	(NANDClose_t) 0x8019CA80, 
	(NANDRead_t) 0x8019B7A4,
	(NANDGetLength_t) 0x8019BF4C}
};
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
	(NANDPrivateOpen_t) 0x8019C88C,
	(NANDClose_t) 0x8019CA80, 
	(NANDRead_t) 0x8019B7A4,
	(NANDGetLength_t) 0x8019BF4C}
};
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
	(NANDPrivateOpen_t) 0x8019C88C,
	(NANDClose_t) 0x8019CA80, 
	(NANDRead_t) 0x8019B7A4,
	(NANDGetLength_t) 0x8019BF4C}
};

void unknownVersion()
{
	// can't do much here!
	// we can't output a message on screen without a valid OSFatal addr
	for (;;);
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
	loadKamekBinaryFromDisc(funcs, "/Binaries/Code.arc", path);
    
    return;
};
