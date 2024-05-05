#ifndef _MUTEX_
#define _MUTEX_
#include "types.hpp"

/* OS Module */
struct OSMutex{
    u8 unknown[0x18];
}; //total size 0x18
void OSInitMutex(OSMutex * mutex);  //801a7eac
void OSLockMutex(OSMutex * mutex);  //801a7ee4
void OSTryLockMutex(OSMutex * mutex);   //801a80f4
void OSUnlockMutex(OSMutex * mutex);    //801a7fc0

#endif

