#ifndef _OS_
#define _OS_
#include "types.hpp"


typedef s32 OSTime;

typedef struct {
	int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
} OSCalendarTime;
void OSTicksToCalendarTime(OSTime time, OSCalendarTime *result);

#define OS_BUS_CLOCK (*(u32 *)0x800000f8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSNanosecondsToTicks(nsec) ((nsec) / (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToNanoseconds(ticks) ((ticks) * (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

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
    void OSShutdownToSBY(); //801ab960
    bool OSGetCurrentThread(); //801a98b0
    bool OSIsTitleInstalled(u64 titleID); //801ae4a0

    u32 OSGetPhysicalMem1Size(); //801a75d0
    u32 OSGetPhysicalMem2Size(); //801a75dc
}
#endif
