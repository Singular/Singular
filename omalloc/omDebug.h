/*******************************************************************
 *  File:    omDebug.h
 *  Purpose: declaration of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omDebug.h,v 1.2 1999-11-22 18:12:58 obachman Exp $
 *******************************************************************/
int omdCheckBinAddr(void* addr, int level);
int omdCheckBlockAddr(void* addr, size_t size, int level);
int omdCheckChunkAddr(void* adr, int level);

int omdCheckBin(omBin bin, int level);
int omdCheckBins(int level);

void* omdCheckAllocBin(omBin bin, int zero,int level);
void  omdCheckFreeBin(void* addr,  int level);
void* omdCheckAllocBlock(size_t size, int zero, int level);
void  omdCheckFreeBlock(void* addr, size_t size, int level);
void* omdCheckAllocChunk(size_t size, int zero, int level);
void omdCheckFreeChunk(void* addr, int level);


