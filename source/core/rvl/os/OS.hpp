#ifndef _OS_
#define _OS_
#include "types.hpp"


typedef s32 OSTime;

typedef struct {
	int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
} OSCalendarTime;
void OSTicksToCalendarTime(OSTime time, OSCalendarTime *result);

extern "C"{
    void OSReport(const char *format, ...);
    OSTime OSGetTime();
    u32 OSGetTick();
    void OSFatal(u32 *fg, u32 *bg, const char *message);
    void OSLaunchTitle(UnkType r3, UnkType r4);
    int OSCalcCRC32(void *data, u32 length);
    int OSDisableInterrupts(); //801a65ac
    int OSEnableInterrupts(); //801a65c0
    int OSRestoreInterrupts(int level); //801a65d4
    int OSDisableScheduler();
    void OSRestart(u32 resetCode);
    void OSShutdownSystem(); //801a856c
    u32 * OSGetStackPointer(); //801a2030
    bool OSIsTitleInstalled(u64 titleID); //801ae4a0
}
#endif
