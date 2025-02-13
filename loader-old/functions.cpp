#include "kamekLoader.hpp"
#include <functions.hpp>

static DisplayFunctions displayFuncsP = {
    0x802a26f0,
    (VIGetNextFrameBuffer_t) 0x801bab24,
    (DirectPrint_ChangeXfb_t) 0x80021e30,
    (DirectPrint_DrawString_t) 0x80021f80,
    (DirectPrint_StoreCache_t) 0x80021e70,
};
static DisplayFunctions displayFuncsE = {
    0x8029e3b0,
    (VIGetNextFrameBuffer_t) 0x801baa84,
    (DirectPrint_ChangeXfb_t) 0x80021d90,
    (DirectPrint_DrawString_t) 0x80021ee0,
    (DirectPrint_StoreCache_t) 0x80021dd0,
};
static DisplayFunctions displayFuncsJ = {
    0x802a2090,
    (VIGetNextFrameBuffer_t) 0x801baa44,
    (DirectPrint_ChangeXfb_t) 0x80021d50,
    (DirectPrint_DrawString_t) 0x80021ea0,
    (DirectPrint_StoreCache_t) 0x80021d90,
};
static DisplayFunctions displayFuncsK = {
    0x802906f0,
    (VIGetNextFrameBuffer_t) 0x801bae80,
    (DirectPrint_ChangeXfb_t) 0x80021e90,
    (DirectPrint_DrawString_t) 0x80021fe0,
    (DirectPrint_StoreCache_t) 0x80021ed0,
};

static DisplayFunctions* displayFunctionsSets[4] = {
    &displayFuncsP, &displayFuncsE, &displayFuncsJ, &displayFuncsK,
};

static IOSFunctions iosFuncsP = {
    (IOS_Open_t) 0x801938f8,
    (IOS_Ioctlv_t) 0x801945e0,
    (IOS_Write_t) 0x80193e88,
    (IOS_Close_t) 0x80193ad8,
};
static IOSFunctions iosFuncsE = {
    (IOS_Open_t) 0x80193858,
    (IOS_Ioctlv_t) 0x80194540,
    (IOS_Write_t) 0x80193de8,
    (IOS_Close_t) 0x80193a38,
};
static IOSFunctions iosFuncsJ = {
    (IOS_Open_t) 0x80193818,
    (IOS_Ioctlv_t) 0x80194500,
    (IOS_Write_t) 0x80193da8,
    (IOS_Close_t) 0x801939f8,
};
static IOSFunctions iosFuncsK = {
    (IOS_Open_t) 0x80193c54,
    (IOS_Ioctlv_t) 0x8019493c,
    (IOS_Write_t) 0x801941e4,
    (IOS_Close_t) 0x80193e34,
};

static IOSFunctions* iosFunctionsSets[4] = {
    &iosFuncsP, &iosFuncsE, &iosFuncsJ,&iosFuncsK,
};

static DVDFunctions dvdFuncsP = {
	(DVDConvertPathToEntrynum_t) 0x8015DF4C,
	(DVDFastOpen_t) 0x8015E254,
	(DVDReadPrio_t) 0x8015E834,
	(DVDClose_t) 0x8015E568,
};
static DVDFunctions dvdFuncsE = {
	(DVDConvertPathToEntrynum_t) 0x8015DEAC,
	(DVDFastOpen_t) 0x8015E1B4,
	(DVDReadPrio_t) 0x8015E794,
	(DVDClose_t) 0x8015E4C8,
};
static DVDFunctions dvdFuncsJ = {
	(DVDConvertPathToEntrynum_t) 0x8015DE6C,
	(DVDFastOpen_t) 0x8015E174,
	(DVDReadPrio_t) 0x8015E754,
	(DVDClose_t) 0x8015E488,
};
static DVDFunctions dvdFuncsK = {
	(DVDConvertPathToEntrynum_t) 0x8015DFC4,
	(DVDFastOpen_t) 0x8015E2CC,
	(DVDReadPrio_t) 0x8015E8AC,
	(DVDClose_t) 0x8015E5E0,
};

static DVDFunctions* dvdFunctionsSets[4] = {
    &dvdFuncsP, &dvdFuncsE, &dvdFuncsJ, &dvdFuncsK,
};

DisplayFunctions* GetDisplayFuncs(){
    return displayFunctionsSets[GetRegionIndex()];
}

IOSFunctions* GetIOSFuncs(){
    return iosFunctionsSets[GetRegionIndex()];
}

DVDFunctions* GetDVDFuncs(){
    return dvdFunctionsSets[GetRegionIndex()];   
}
