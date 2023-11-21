#ifndef _BIT_OPS_H
#define _BIT_OPS_H

#include <kamek.hpp>

#define __lhbrx(base) __extension__		\
  ({unsigned short result;	       		\
    typedef  struct {char a[2];} halfwordsize;	\
    halfwordsize *ptrp = (halfwordsize*)(void*)(base);	\
  __asm__ ("lhbrx %0,%y1"			\
	   : "=r" (result)			\
	   : "Z" (*ptrp));			\
  result; })

#define __lwbrx(base) __extension__		\
  ({unsigned int result;	       		\
    typedef  struct {char a[4];} wordsize;	\
    wordsize *ptrp = (wordsize*)(void*)(base);		\
  __asm__ ("lwbrx %0,%y1"			\
	   : "=r" (result)			\
	   : "Z" (*ptrp));			\
  result; })

 #define __sthbrx(base, value) do {			\
    typedef  struct {char a[2];} halfwordsize;		\
    halfwordsize *ptrp = (halfwordsize*)(void*)(base);		\
    __asm__ ("sthbrx %1,%y0"				\
	   : "=Z" (*ptrp)				\
	   : "r" (value));				\
   } while (0)

#define __stwbrx(base, value) do {		\
    typedef  struct {char a[4];} wordsize;	\
    wordsize *ptrp = (wordsize*)(void*)(base);		\
    __asm__ ("stwbrx %1,%y0"			\
	   : "=Z" (*ptrp)			\
	   : "r" (value));			\
   } while (0)
 

static inline u16 u8array_to_u16(const u8 *array, int offset) {
    return __lhbrx(array + offset);
}
static inline u32 u8array_to_u32(const u8 *array, int offset) {
    return __lwbrx(array + offset);
}
static inline void u16_to_u8array(u8 *array, int offset, u16 value) {
    __sthbrx(array + offset, value);
}
static inline void u32_to_u8array(u8 *array, int offset, u32 value) {
    __stwbrx(array + offset, value);
}

#endif