#ifndef OGC_DISC_IO_INCLUDE
#define OGC_DISC_IO_INCLUDE

#include <kamek.hpp>

#define FEATURE_MEDIUM_CANREAD      0x00000001
#define FEATURE_MEDIUM_CANWRITE     0x00000002
#define FEATURE_GAMECUBE_SLOTA      0x00000010
#define FEATURE_GAMECUBE_SLOTB      0x00000020
#define FEATURE_GAMECUBE_DVD        0x00000040
#define FEATURE_WII_SD              0x00000100
#define FEATURE_WII_USB             0x00000200
#define FEATURE_WII_DVD             0x00000400

typedef u32 sec_t;

typedef bool (* FN_MEDIUM_STARTUP)(void) ;
typedef bool (* FN_MEDIUM_ISINSERTED)(void) ;
typedef bool (* FN_MEDIUM_READSECTORS)(sec_t sector, sec_t numSectors, void* buffer) ;
typedef bool (* FN_MEDIUM_WRITESECTORS)(sec_t sector, sec_t numSectors, const void* buffer) ;
typedef bool (* FN_MEDIUM_CLEARSTATUS)(void) ;
typedef bool (* FN_MEDIUM_SHUTDOWN)(void) ;

typedef struct DISC_INTERFACE_STRUCT {
	u32			ioType ;
	u32			features ;
	FN_MEDIUM_STARTUP		startup ;
	FN_MEDIUM_ISINSERTED	isInserted ;
	FN_MEDIUM_READSECTORS	readSectors ;
	FN_MEDIUM_WRITESECTORS	writeSectors ;
	FN_MEDIUM_CLEARSTATUS	clearStatus ;
	FN_MEDIUM_SHUTDOWN		shutdown ;
} DISC_INTERFACE;

#endif