/*******************************************************************
 *  File:    omCheck.h
 *  Purpose: declaration of omCheck functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omCheck.h,v 1.2 2000-05-31 13:34:30 obachman Exp $
 *******************************************************************/

#include "omError.h"

/* Checking addresses and Bins */
omError_t omCheckBinAddr(void* addr, int level);
omError_t omCheckBinAddrBin(void* addr, omBin bin, int level);
omError_t omCheckAddr(void* adr, int level);
omError_t omCheckAddrSize(void* addr, size_t size, int level);

omError_t omCheckAlignedBinAddr(void* addr, int level);
omError_t omCheckAlignedBinAddrBin(void* addr, omBin bin, int level);
omError_t omCheckAlignedAddr(void* adr, int level);
omError_t omCheckAlignedAddrSize(void* addr, size_t size, int level);

omError_t omCheckBin(omBin bin, int level);
omError_t omCheckBins(int level);

/* combining check and alloc/free */
void* omCheckAllocBin(omBin bin, int level);
void* omCheckAlloc0Bin(omBin bin, int level);
void* omCheckReallocBin(void* addr, omBin old_bin, omBin new_bin, int level);
void* omCheckRealloc0Bin(void* addr, omBin old_bin, omBin new_bin, int level);
void  omCheckFreeBin(void* addr, omBin bin, int level);

void* omCheckAlloc(size_t Size, int level);
void* omCheckAlloc0(size_t Size, int level);
void* omCheckAllocAligned(size_t Size, int level);
void* omCheckAlloc0Aligned(size_t Size, int level);

void* omCheckRealloc(void* addr, size_t new_Size, int level);
void* omCheckRealloc0(void* addr, size_t new_Size, int level);
void* omCheckReallocAligned(void* addr, size_t new_Size, int level);
void* omCheckRealloc0Aligned(void* addr, size_t new_Size, int level);

void* omCheckReallocSize(void* addr, size_t old_Size, size_t new_Size, int level);
void* omCheckRealloc0Size(void* addr, size_t old_Size, size_t new_Size, int level);
void* omCheckReallocAlignedSize(void* addr, size_t old_Size, size_t new_Size, int level);
void* omCheckRealloc0AlignedSize(void* addr, size_t old_Size, size_t new_Size, int level);

void omCheckFreeSize(void* addr, size_t size, int level);
void omCheckFree(void* addr, int level);

void* omCheckAllocAlignedFunc(size_t size);
void* omCheckReallocAlignedFunc(void* addr, size_t new_size);
void  omCheckFreeFunc(void* addr);

/* used internally: verify if addr is from bin page
   by iterating through all used bin pages */
int omIsAddrFromBinPage(void* addr);




