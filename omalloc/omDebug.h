/*******************************************************************
 *  File:    omDebug.h
 *  Purpose: declaration of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omDebug.h,v 1.3 2000-05-31 13:34:31 obachman Exp $
 *******************************************************************/
#include "omError.h"

/* Debugging addresses and Bins */
omError_t omDebugAddrBin(void* addr, omBin bin, int level);
omError_t omDebugAddr(void* adr, int level);
omError_t omDebugAddrSize(void* addr, size_t size, int level);

omError_t omDebugAlignedBinAddr(void* addr, int level);
omError_t omDebugAlignedBinAddrBin(void* addr, omBin bin, int level);
omError_t omDebugAlignedAddr(void* adr, int level);
omError_t omDebugAlignedAddrSize(void* addr, size_t size, int level);

omError_t omDebugBin(omBin bin, int level);
omError_t omDebugBins(int level);

/* combining check and alloc/free */
void* omDebugAllocBin(omBin bin, int level);
void* omDebugAlloc0Bin(omBin bin, int level);
void* omDebugReallocBin(void* addr, omBin old_bin, omBin new_bin, int level);
void* omDebugRealloc0Bin(void* addr, omBin old_bin, omBin new_bin, int level);
void  omDebugFreeBin(void* addr, omBin bin, int level);

void* omDebugAlloc(size_t Size, int level);
void* omDebugAlloc0(size_t Size, int level);
void* omDebugAllocAligned(size_t Size, int level);
void* omDebugAlloc0Aligned(size_t Size, int level);

void* omDebugRealloc(void* addr, size_t new_Size, int level);
void* omDebugRealloc0(void* addr, size_t new_Size, int level);
void* omDebugReallocAligned(void* addr, size_t new_Size, int level);
void* omDebugRealloc0Aligned(void* addr, size_t new_Size, int level);

void* omDebugReallocSize(void* addr, size_t old_Size, size_t new_Size, int level);
void* omDebugRealloc0Size(void* addr, size_t old_Size, size_t new_Size, int level);
void* omDebugReallocAlignedSize(void* addr, size_t old_Size, size_t new_Size, int level);
void* omDebugRealloc0AlignedSize(void* addr, size_t old_Size, size_t new_Size, int level);

void omDebugFreeSize(void* addr, size_t size, int level);
void omDebugFree(void* addr, int level);

void* omDebugAllocAlignedFunc(size_t size);
void* omDebugReallocAlignedFunc(void* addr, size_t new_size);
void  omDebugFreeFunc(void* addr);







