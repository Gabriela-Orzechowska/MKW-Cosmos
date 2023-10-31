#ifndef _MUTEX_
#define _MUTEX_
#include "types.hpp"

typedef struct OSMutex
{
    void * queue;
    void * thread;
    s32 count;
    OSMutex * next;
    OSMutex * prev;

} OSMutex;

void OSInitMutex(OSMutex * mutex);  //801a7eac
void OSLockMutex(OSMutex * mutex);  //801a7ee4
void OSTryLockMutex(OSMutex * mutex);   //801a80f4
void OSUnlockMutex(OSMutex * mutex);    //801a7fc0

#endif

