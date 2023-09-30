#ifndef _EGGHEAP_
#define _EGGHEAP_
#include "types.hpp"
#include <core/rvl/allocator.h>
#include <core/rvl/os/thread.hpp>
#include <core/rvl/mtx/mtx.hpp>
#include <core/nw4r/ut/List.hpp>
#include <core/egg/Disposer.hpp>

namespace EGG {
	class Heap;
	class Allocator : public MEMAllocator {
		public:
			Allocator(Heap *heap, s32 alignment); //8020f62c
			virtual ~Allocator(); //8020f688 vtable 802a2668
			virtual void *alloc(u32 size); //8020f6c8
			virtual void free(void *buffer); //8020f6dc

		private:
			Heap *heap;
			s32 alignment;
	};

	class Heap : public Disposer {
	public:
		Heap(/*MEMiHeapHead*/ void *rvlHeap); //802296e8
		virtual ~Heap(); //80229780 vtable 802a30c0
		virtual int getHeapKind() const = 0;
		virtual void initAllocator(Allocator *allocator, s32 alignment) = 0;
		virtual void *alloc(u32 size, s32 alignment) = 0;
		virtual void free(void *buffer) = 0;
		virtual void destroy() = 0; //0x1c
		virtual u32 resizeForMBlock(void *buffer, u32 newSize) = 0;
		virtual u32 getAllocatableSize(s32 alignment) = 0;


		static void *alloc(u32 size, int align, Heap *heap);
		static Heap *findHeap(/*MEMiHeapHead*/ void *rvlHeap);
		Heap *findParentHeap();
		
		static Heap *findContainHeap(void *buffer);
		static void free(void *buffer, Heap *heap);
		void dispose(); //destroys disposers
		void dump();
		static void dumpAll();
		Heap *BecomeCurrentHeap(); //returns old heap
		Heap *becomeCurrentHeapWithoutLocking();
	private:
		nw4r::ut::Link parentLink;
		void *rvlHeap; //0x10
		void *parentHeapMBlock; //0x14
		Heap *parentHeap; //0x18
		u16 dameFlag; //0x1C
		nw4r::ut::Link globalLink; //0x20
		nw4r::ut::List childList; //0x28
		const char *name; //0x34
	}; //total size 0x38

	class ExpHeap : public Heap{
	public:
		static ExpHeap *Create(int size, Heap *parent, u16 flags);
		ExpHeap(); //80226a1c
		virtual ~ExpHeap(); //0x8 802269a8 vtable 802a2ff8
		int getHeapKind() const override; //C 80226efc
		void initAllocator(Allocator *allocator, s32 alignment) override; //0x10 80226dc0
		void *alloc(u32 size, s32 alignment) override; //0x14 80226c04
		void free(void *buffer) override; //0x18 80226c78
		void destroy() override; //0x1c 80226b94
		u32 resizeForMBlock(void *buffer, u32 newSize) override; //0x20 80226c80
		u32 getAllocatableSize(s32 alignment) override; //0x24 80226c90
		virtual u32 adjust(); //0x28 80226d4c
		u32 getTotalFreeSize(); //80226c88

	};
}

#ifdef __INTELLISENSE__
inline void * operator new(unsigned long long size, EGG::Heap *heap)
{
	return EGG::Heap::alloc(size, 0x4, heap);
}
inline void * operator new[](unsigned long long size, EGG::Heap *heap)
{
    return EGG::Heap::alloc(size, 0x4, heap);
}
inline void * operator new(unsigned long long size, u32 alignment)
{
	return EGG::Heap::alloc(size, alignment, 0);
}
inline void * operator new[](unsigned long long size, u32 alignment)
{
    return EGG::Heap::alloc(size, alignment, 0);
}
inline void * operator new(unsigned long long size, EGG::Heap *heap, u32 alignment)
{
	return EGG::Heap::alloc(size, alignment, heap);
}
inline void * operator new[](unsigned long long size, EGG::Heap *heap, u32 alignment)
{
    return EGG::Heap::alloc(size, alignment, heap);
}
inline void * operator new(unsigned long long,  void *ptr) { return ptr; }
#else
inline void * operator new(size_t size, EGG::Heap *heap)
{
	return EGG::Heap::alloc(size, 0x4, heap);
}
inline void * operator new[](size_t size, EGG::Heap *heap)
{
    return EGG::Heap::alloc(size, 0x4, heap);
}
inline void * operator new(size_t size, u32 alignment)
{
	return EGG::Heap::alloc(size, alignment, 0);
}
inline void * operator new[](size_t size, u32 alignment)
{
    return EGG::Heap::alloc(size, alignment, 0);
}
inline void * operator new(size_t size, EGG::Heap *heap, u32 alignment)
{
	return EGG::Heap::alloc(size, alignment, heap);
}
inline void * operator new[](size_t size, EGG::Heap *heap, u32 alignment)
{
    return EGG::Heap::alloc(size, alignment, heap);
}
inline void * operator new(size_t,  void *ptr) { return ptr; }//namespace EGG 
#endif
#endif