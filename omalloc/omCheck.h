/*******************************************************************
 *  File:    omCheck.h
 *  Purpose: declaration of omCheck functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omCheck.h,v 1.1 1999-11-23 20:40:12 obachman Exp $
 *******************************************************************/
int omCheckBinAddr(void* addr, int level);
int omCheckBlockAddr(void* addr, size_t size, int level);
int omCheckChunkAddr(void* adr, int level);

int omCheckBin(omBin bin, int level);
int omCheckBins(int level);

void* omCheckAllocBin(omBin bin, int zero,int level);
void  omCheckFreeBin(void* addr,  int level);
void* omCheckAllocBlock(size_t size, int zero, int level);
void  omCheckFreeBlock(void* addr, size_t size, int level);
void* omCheckAllocChunk(size_t size, int zero, int level);
void  omCheckFreeChunk(void* addr, int level);


