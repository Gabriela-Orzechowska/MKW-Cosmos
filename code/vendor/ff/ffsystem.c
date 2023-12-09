/*------------------------------------------------------------------------*/
/* A Sample Code of User Provided OS Dependent Functions for FatFs        */
/*------------------------------------------------------------------------*/

#include <vendor/ff/ff.h>

typedef int UnkType;
typedef signed long long OSTime;

typedef struct {
	int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
} OSCalendarTime;
void OSTicksToCalendarTime(OSTime time, OSCalendarTime *result);
void OSReport(const char *format, ...);
OSTime OSGetTime();

DWORD get_fattime()
{
	OSCalendarTime time;
	OSTicksToCalendarTime(OSGetTime(), &time);

	DWORD val = 0;

	val |= (time.year - 1980) << 25;
	val |= (time.mon + 1) << 21;
	val |= time.mday << 16;
	val |= time.hour << 11;
	val |= time.min << 5;

	if (time.sec >= 60) {
        val |= 59 >> 1;
    } else {
        val |= time.sec >> 1;
    }

    return val;
}