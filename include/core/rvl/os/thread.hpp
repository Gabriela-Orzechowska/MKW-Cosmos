#ifndef _OSTHREAD_
#define _OSTHREAD_
#include "types.hpp"
#include "core/rvl/os/mutex.hpp"    

struct OSContext{
    u32 gpr[32]; //from 0 to c8
    u32 cr;
    u32 lr; //0x84
    u32 ctr; //0x88
    u32 xer;
    f64 fpr[32]; //ends at 0x190
    f64 fpscr; 
    u32 srr0; //0x198
    u32 srr1; //0x19C
    u16 unknown_0x1A0;
    u16 unknown_0x1A2;
    u32 gqr[8]; //0x1A4 to 0x1C4
    u32 unknown_0x1C4;
    f64 pairedSinglesRegisters[32];
}; //total size 0x2C8

typedef void* OSMessage;

class OSThread;

struct OSThreadQueue {
	OSThread *head;
	OSThread *tail;
};

struct OSMessageQueue{
    OSThreadQueue sendQueue;
    OSThreadQueue recvQueue;
    OSMessage *messages; //0x10
    s32 msgCount; //0x14
    s32 messageIndex; //0x18
    s32 used; //0x1C
}; //total size 0x20


class OSThread{ //priority between 0 and 31
    //no ctor
    OSContext context; //arg from ctor stored at C
    u16 unknown_0x2C8;
    u16 detached;
    u32 unknown_0x2CC;
    u32 priority;
    u32 priority2;
    u32 unknown_0x2D8;
    OSThreadQueue * threadQueue;
    OSThread *next;
    OSThread *prev;
    OSThreadQueue joinAndDetachQueue;
    OSMutex * mutexToLock;
    OSMutex * mutexHead;
    OSMutex * mutexTail;
    OSThread *activeNext;
    OSThread *activePrev;
    void *stackStart; //0x304 higher value
    void *stackEnd; //lower value
    u32 unknown_0x30C[3];
    static void OSCreateThread(OSThread *thread, void (*runFunc)(void *arg), void *arg, void *stack, u32 stackSize, u32 priority, u16 detached); //801a9e84
}; 
size_assert(OSThread, 0x318);

extern "C"{
    OSContext *OSGetCurrentContext(); //801a1ecc
    int OSSendMessage(OSMessageQueue *queue, OSMessage msg, s32 flags);
    void OSInitMessageQueue(OSMessageQueue *queue, OSMessage* msgArray, s32 msgCount );
    void OSYieldThread();
    void CancelThread();
}
#endif