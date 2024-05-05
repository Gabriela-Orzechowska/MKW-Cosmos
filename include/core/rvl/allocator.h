#ifndef _RVL_ALLOCATOR_
#define _RVL_ALLOCATOR_
#include "types.hpp"

typedef struct MEMAllocator MEMAllocator;
typedef void* (*MEMFuncAllocatorAlloc)(MEMAllocator *allocator, u32 size );
typedef void  (*MEMFuncAllocatorFree) (MEMAllocator *allocator, void* memBlock);

struct MEMAllocatorFunc{
    MEMFuncAllocatorAlloc alloc;
    MEMFuncAllocatorFree free;
};

struct MEMAllocator{
    MEMAllocatorFunc const *func;
    void* heap;
    u32 heapParam1;
    u32 heapParam2;
}; //0x10

#endif