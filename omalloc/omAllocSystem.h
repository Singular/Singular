/*******************************************************************
 *  File:    omAllocSystem.h
 *  Purpose: declaration of routines for low-level alloc routines
 *           and page management
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_ALLOC_SYSTEM_H
#define OM_ALLOC_SYSTEM_H

size_t omSizeOfAddr(const void* addr);
size_t omSizeWOfAddr(void* addr);

size_t omSizeOfLargeAddr(void* addr);
#define omSizeWOfLargeAddr(addr) (omSizeOfLargeAddr(addr) >> LOG_SIZEOF_LONG)

void* omAllocFromSystem(size_t size);
void* omReallocFromSystem(void* addr, size_t newsize);
void  omFreeToSystem(void* addr);

void* omReallocSizeFromSystem(void* addr, size_t oldsize, size_t newsize);
void  omFreeSizeToSystem(void* addr, size_t size);

#define omVallocFromSystem(size) _omVallocFromSystem(size, 0)
void* _omVallocFromSystem(size_t size, int fail);
void omVfreeToSystem(void* page, size_t size);


#ifdef OM_MALLOC_PROVIDES_SIZEOF_ADDR
#define omAllocLarge(size)             omAllocFromSystem(OM_ALIGN_SIZE(size))
#define omReallocLarge(addr, new_size) omReallocSizeFromSystem(addr, omSizeOfLargeAddr(addr), OM_ALIGN_SIZE(new_size))
#define omFreeLarge(addr)              omFreeSizeToSystem(addr, omSizeOfLargeAddr(addr))
#else
void* omAllocLarge(size_t size);
void* omReallocLarge(void* old_addr, size_t new_size);
void  omFreeLarge(void* addr);
#endif
void* omRealloc0Large(void* old_addr, size_t new_size);
void* omAlloc0Large(size_t size);

#endif /* OM_ALLOC_SYSTEM_H */
