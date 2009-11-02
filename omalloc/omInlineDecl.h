/*******************************************************************
 *  File:    omInlineDecl.h
 *  Purpose: declarations of omalloc functions which could
 *           be inlined
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_INLINE_DECL_H
#define OM_INLINE_DECL_H

OM_INLINE_DECL omBin omGetBinOfPage(omBinPage page);
OM_INLINE_DECL int _omIsBinPageAddr(const void* addr);

OM_INLINE_DECL void* _omAllocBin(omBin bin);
OM_INLINE_DECL void* _omAlloc0Bin(omBin bin);
OM_INLINE_DECL void* _omReallocBin(void* addr, omBin old_bin, omBin new_bin);
OM_INLINE_DECL void* _omRealloc0Bin(void* addr, omBin old_bin, omBin new_bin);

OM_INLINE_DECL void* _omAlloc(size_t size);
OM_INLINE_DECL void* _omAlloc0(size_t size);
OM_INLINE_DECL void* _omReallocSize(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omRealloc0Size(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omRealloc(void* addr, size_t size);
OM_INLINE_DECL void* _omRealloc0(void* addr, size_t size);

OM_INLINE_DECL void* _omalloc(size_t size);
OM_INLINE_DECL void* _omalloc0(size_t size);
OM_INLINE_DECL void* _omreallocSize(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omrealloc0Size(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omrealloc(void* addr, size_t size);
OM_INLINE_DECL void* _omrealloc0(void* addr, size_t size);

OM_INLINE_DECL char* _omStrDup(const char* s);
OM_INLINE_DECL void* _omMemDup(void* addr);

#ifdef OM_ALIGNMENT_NEEDS_WORK
OM_INLINE_DECL void* _omAllocAligned(size_t size);
OM_INLINE_DECL void* _omAlloc0Aligned(size_t size);
OM_INLINE_DECL void* _omReallocAlignedSize(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omRealloc0AlignedSize(void* addr, size_t old_size, size_t new_size);
OM_INLINE_DECL void* _omReallocAligned(void* addr, size_t size);
OM_INLINE_DECL void* _omRealloc0Aligned(void* addr, size_t size);
OM_INLINE_DECL void* _omMemDupAligned(void* addr);
#endif

#endif /* OM_INLINE_DECL_H */
