#ifndef __WIISD_IO_H__
#define __WIISD_IO_H__

#include <modules/disc_io/disc_io.hpp>

#define DEVICE_TYPE_WII_SD (('W'<<24)|('I'<<16)|('S'<<8)|'D')

extern const DISC_INTERFACE __io_wiisd;

#endif