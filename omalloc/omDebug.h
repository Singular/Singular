/*******************************************************************
 *  File:    omDebug.h
 *  Purpose: declaration of main omDebug functions
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omDebug.h,v 1.1.1.1 1999-11-18 17:45:53 obachman Exp $
 *******************************************************************/
int omdCheckBinAddr(void* addr, omBin bin, 
                    int level, const char* fn, const int l);
int omdCheckBlockAddr(void* addr, size_t size,  
                      int level, const char* fn, const int l);
int omCheckChunkAddr(void* adr, const char* fn, const int l);

int omdCheckBin(omBin bin, int level, const char* fn, const int l);
int omdCheckBins(int level, const char* fn, const int l);

void* omdCheckAllocBin(omBin bin, int zero, 
                       int level, const char* fn, const int l);
void  omdCheckFreeBin(void* addr,  omBin bin, 
                      int level, const char* fn, int l);
void* omdCheckAllocBlock(size_t size, int zero,  
                         int level, const char* fn, const int l);
void  omdCheckFreeBlock(void* addr, size_t size,  
                        int level, const char* fn, const int l);
void* omdCheckAllocChunk(size_t size, int zero,  
                         int level, const char* fn, const int l);
void omdCheckFreeChunk(void* addr, int level, const char* fn, const int l);


