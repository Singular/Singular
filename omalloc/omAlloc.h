/*******************************************************************
 *  File:    omAlloc.h
 *  Purpose: declaration of public routines for omalloc  
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAlloc.h,v 1.2 1999-11-26 17:57:52 obachman Exp $
 *******************************************************************/
#ifndef OM_ALLOC_H
#define OM_ALLOC_H

#ifndef OM_USE_EXTERNAL_MALLOC
/*******************************************************************
 * forward type declarations
 *******************************************************************/
struct omBin_s;
struct omSpecBin_s;
typedef struct omBin_s      omBin_t;
typedef struct omSpecBin_s  omSpecBin_t;
typedef omBin_t*            omBin;
typedef omSpecBin_t*        omSpecBin;

/*******************************************************************
 *  
 *  Alloc/Free Func
 *  
 *******************************************************************/
extern void* omFuncAllocBin(omBin bin);
extern void* omFuncAlloc0Bin(omBin bin);
extern void  omFuncFreeBin(void* addr);
extern void  omFuncPseudoFreeBin(void* addr);

extern void* omFuncAllocBlock(size_t size);
extern void* omFuncAlloc0Block(size_t size);
extern void* omFuncReallocBlock(void* addr, size_t old_size, size_t new_size);
extern void* omFuncReallo0cBlock(void* addr, size_t old_size, size_t new_size);
extern void* omFuncAllocAlignedBlock(size_t size);
extern void* omFuncAlloc0AlignedBlock(size_t size);
extern void* omFuncReallocAlignedBlock(void* addr,size_t old_size,size_t new_size);
extern void* omFuncRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size);
extern void  omFuncFreeBlock(void* addr, size_t size);
extern void* omFuncPseudoReallocBlock(void* addr, size_t old_size, size_t new_size);
extern void* omFuncPseudoReallo0cBlock(void* addr, size_t old_size, size_t new_size);
extern void* omFuncPseudoReallocAlignedBlock(void* addr,size_t old_size,size_t new_size);
extern void* omFuncPseudoRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size);
extern void  omFuncPseudoFreeBlock(void* addr, size_t size);

extern void* omFuncAllocChunk(size_t size);
extern void* omFuncAlloc0Chunk(size_t size);
extern void* omFuncAllocAlignedChunk(size_t size);
extern void* omFuncAlloc0AlignedChunk(size_t size);
extern void* omFuncReallocChunk(void* addr, size_t size);
extern void* omFuncReallocAlignedChunk(void* addr, size_t size);
extern void  omFuncFreeChunk(void* addr);
extern void  omFuncFreeAlignedChunk(void* addr);
extern void* omFuncPseudoReallocChunk(void* addr, size_t size);
extern void* omFuncPseudoReallocAlignedChunk(void* addr, size_t size);
extern void  omFuncPseudoFreeChunk(void* addr);
extern void  omFuncPseudoFreeAlignedChunk(void* addr);

/*******************************************************************
 *  
 *  Alloc/Free Check
 *  
 *******************************************************************/
extern void* omCheckAllocBin(omBin bin, int level);
extern void* omCheckAlloc0Bin(omBin bin, int level);
extern void  omCheckFreeBin(void* addr, int level);
extern void  omCheckPseudoFreeBin(void* addr, int level);

extern void* omCheckAllocBlock(size_t size, int level);
extern void* omCheckAlloc0Block(size_t size, int level);
extern void* omCheckReallocBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omCheckReallo0cBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omCheckAllocAlignedBlock(size_t size, int level);
extern void* omCheckAlloc0AlignedBlock(size_t size, int level);
extern void* omCheckReallocAlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void* omCheckRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void  omCheckFreeBlock(void* addr, size_t size, int level);
extern void* omCheckPseudoReallocBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omCheckPseudoReallo0cBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omCheckPseudoReallocAlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void* omCheckPseudoRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void  omCheckPseudoFreeBlock(void* addr, size_t size, int level);

extern void* omCheckAllocChunk(size_t size, int level);
extern void* omCheckAlloc0Chunk(size_t size, int level);
extern void* omCheckAllocAlignedChunk(size_t size, int level);
extern void* omCheckAlloc0AlignedChunk(size_t size, int level);
extern void* omCheckReallocChunk(void* addr, size_t size, int level);
extern void* omCheckReallocAlignedChunk(void* addr, size_t size, int level);
extern void  omCheckFreeChunk(void* addr, int level);
extern void  omCheckFreeAlignedChunk(void* addr, int level);
extern void* omCheckPseudoReallocChunk(void* addr, size_t size, int level);
extern void* omCheckPseudoReallocAlignedChunk(void* addr, size_t size, int level);
extern void  omCheckPseudoFreeChunk(void* addr, int level);
extern void  omCheckPseudoFreeAlignedChunk(void* addr, int level);

/* Check of addr/bins -- return NULL if ok, error message, otherwise */
extern char* omCheckBinAddr(void* addr, int level);
extern char* omCheckBin(omBin bin, int level);
extern char* omCheckAllBins(int level);
extern char* omCheckBlockAddr(void* addr, size_t size, int level);
extern char* omCheckAlignedBlockAddr(void* addr, size_t size, int level);
extern char* omCheckChunkAddr(void* addr);
extern char* omCheckAlignedChunkAddr(void* addr);

/* CheckReport of addr/bins -- return 1 if ok, otherwise print error and backtrace to stdout
   and return 0 */
extern int omCheckReportBinAddr(void* addr, int level);
extern int omCheckReportBin(omBin bin, int level);
extern int omCheckReportAllBins(int level);
extern int omCheckReportBlockAddr(void* addr, size_t size, int level);
extern int omCheckReportAlignedBlockAddr(void* addr, size_t size, int level);
extern int omCheckReportChunkAddr(void* addr);
extern int omCheckReportAlignedChunkAddr(void* addr);

/*******************************************************************
 *  
 *  Alloc/Free Debug
 *  
 *******************************************************************/
extern void* omDebugAllocBin(omBin bin, int level);
extern void* omDebugAlloc0Bin(omBin bin, int level);
extern void  omDebugFreeBin(void* addr, int level);
extern void  omDebugPseudoFreeBin(void* addr, int level);

extern void* omDebugAllocBlock(size_t size, int level);
extern void* omDebugAlloc0Block(size_t size, int level);
extern void* omDebugReallocBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omDebugReallo0cBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omDebugAllocAlignedBlock(size_t size, int level);
extern void* omDebugAlloc0AlignedBlock(size_t size, int level);
extern void* omDebugReallocAlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void* omDebugRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void  omDebugFreeBlock(void* addr, size_t size, int level);
extern void* omDebugPseudoReallocBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omDebugPseudoReallo0cBlock(void* addr, size_t old_size, size_t new_size, int level);
extern void* omDebugPseudoReallocAlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void* omDebugPseudoRealloc0AlignedBlock(void* addr,size_t old_size,size_t new_size, int level);
extern void  omDebugPseudoFreeBlock(void* addr, size_t size, int level);

extern void* omDebugAllocChunk(size_t size, int level);
extern void* omDebugAlloc0Chunk(size_t size, int level);
extern void* omDebugAllocAlignedChunk(size_t size, int level);
extern void* omDebugAlloc0AlignedChunk(size_t size, int level);
extern void* omDebugReallocChunk(void* addr, size_t size, int level);
extern void* omDebugReallocAlignedChunk(void* addr, size_t size, int level);
extern void  omDebugFreeChunk(void* addr, int level);
extern void  omDebugFreeAlignedChunk(void* addr, int level);
extern void* omDebugPseudoReallocChunk(void* addr, size_t size, int level);
extern void* omDebugPseudoReallocAlignedChunk(void* addr, size_t size, int level);
extern void  omDebugPseudoFreeChunk(void* addr, int level);
extern void  omDebugPseudoFreeAlignedChunk(void* addr, int level);

/* Debug of addr/bins -- return NULL if ok, error message, otherwise */
extern char* omDebugBinAddr(void* addr, int level);
extern char* omDebugBin(omBin bin, int level);
extern char* omDebugAllBins(int level);
extern char* omDebugBlockAddr(void* addr, size_t size, int level);
extern char* omDebugAlignedBlockAddr(void* addr, size_t size, int level);
extern char* omDebugChunkAddr(void* addr);
extern char* omDebugAlignedChunkAddr(void* addr);

/* DebugReport of addr/bins -- return 1 if ok, otherwise print error and current stack 
   backtrace to stderr and return 0 */
extern int omDebugReportBinAddr(void* addr, int level);
extern int omDebugReportBin(omBin bin, int level);
extern int omDebugReportAllBins(int level);
extern int omDebugReportBlockAddr(void* addr, size_t size, int level);
extern int omDebugReportAlignedBlockAddr(void* addr, size_t size, int level);
extern int omDebugReportChunkAddr(void* addr);
extern int omDebugReportAlignedChunkAddr(void* addr);

/*******************************************************************
 *  
 *  om Alloc/Free Macro definitions
 *  
 *******************************************************************/

#if definded(OM_DEBUG) && OM_DEBUG != 0 
/*******************************************************************
 * Alloc/Free for Debug
 *******************************************************************/

#define omTypeAllocBin(type, addr, bin)                     addr = (type) omDebugAllocBin(bin, OM_DEBUG)
#define omTypeAlloc0Bin(type, addr, bin)                    addr = (type) omDebugAlloc0Bin(bin, OM_DEBUG)
#define omAllocBin(bin)                                     omDebugAllocBin(bin, OM_DEBUG)
#define omAlloc0Bin(bin)                                    omDebugAlloc0Bin(bin, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omFreeBin(addr)                                     omDebugPseudoFreeBin(addr, OM_DEBUG)
#else
#define omFreeBin(addr)                                     omDebugFreeBin(addr, OM_DEBUG)
#endif

#define omTypeAllocBlock(type, addr, size)                  addr = (type) omDebugAllocBlock(size, OM_DEBUG)
#define omTypeAlloc0Block(type, addr, size)                 addr = (type) omDebugAlloc0Block(size, OM_DEBUG)
#define omAllocBlock(size)                                  omDebugAllocBlock(size, OM_DEBUG)
#define omAlloc0Block(size)                                 omDebugAlloc0Block(size, OM_DEBUG)
#define omTypeAllocAlignedBlock(type, addr, size)           addr = (type) omDebugAllocAlignedBlock(size, OM_DEBUG)
#define omTypeAlloc0AlignedBlock(type, addr, size)          addr = (type) omDebugAlloc0AlignedBlock(size, OM_DEBUG)
#define omAllocAlignedBlock(size)                           omDebugAllocAlignedBlock(size, OM_DEBUG)
#define omAlloc0AlignedBlock(size)                          omDebugAlloc0AlignedBlock(size, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocBlock(addr, old_size, new_size)            omDebugPseudoReallocBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0Block(addr, old_size, new_size)           omDebugPseudoRealloc0Block(addr, old_size, new_size, OM_DEBUG)
#define omReallocAlignedBlock(addr, old_size, new_size)     omDebugPseudoReallocAlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    omDebugPseudoRealloc0AlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omFreeBlock(addr, size)                             omDebugPseudoFreeBlock(addr, size, OM_DEBUG)
#else
#define omReallocBlock(addr, old_size, new_size)            omDebugReallocBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0Block(addr, old_size, new_size)           omDebugRealloc0Block(addr, old_size, new_size, OM_DEBUG)
#define omReallocAlignedBlock(addr, old_size, new_size)     omDebugReallocAlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    omDebugRealloc0AlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omFreeBlock(addr, size)                             omDebugFreeBlock(addr, size, OM_DEBUG)
#endif

#define omTypeAllocChunk(type, addr, size)                  addr = (type) omDebugAllocChunk(size, OM_DEBUG)
#define omTypeAlloc0Chunk(type, addr, size)                 addr = (type) omDebugAlloc0Chunk(size, OM_DEBUG)
#define omAllocChunk(size)                                  omDebugAllocChunk(size, OM_DEBUG)
#define omAlloc0Chunk(size)                                 omDebugAlloc0Chunk(size, OM_DEBUG)
#define omTypeAllocAlignedChunk(type, addr, size)           addr = (type) omDebugAllocAlignedChunk(size, OM_DEBUG)
#define omTypeAlloc0AlignedChunk(type, addr, size)          addr = (type) omDebugAlloc0AlignedChunk(size, OM_DEBUG)
#define omAllocAlignedChunk(size)                           omDebugAllocAlignedChunk(size, OM_DEBUG)
#define omAlloc0AlignedChunk(size)                          omDebugAlloc0AlignedChunk(size, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocChunk(addr, new_size)                      omDebugPseudoReallocChunk(addr, new_size, OM_DEBUG)
#define omReallocAlignedChunk(addr, new_size)               omDebugPseudoReallocAlignedChunk(addr, new_size, OM_DEBUG)
#define omFreeChunk(addr)                                   omDebugPseudoFreeChunk(addr, OM_DEBUG)
#define omFreeAlignedChunk(addr)                            omDebugPseudoFreeAlignedChunk(addr, OM_DEBUG)
#else
#define omReallocChunk(addr, new_size)                      omDebugReallocChunk(addr, new_size, OM_DEBUG)
#define omReallocAlignedChunk(addr, new_size)               omDebugReallocAlignedChunk(addr, new_size, OM_DEBUG)
#define omFreeChunk(addr)                                   omDebugFreeChunk(addr, OM_DEBUG)
#define omFreeAlignedChunk(addr)                            omDebugFreeAlignedChunk(addr, OM_DEBUG)
#endif


#define omTestBinAddr(addr)                                 omDebugBinAddr(addr, OM_DEBUG)
#define omTestBlockAddr(addr, size)                         omDebugBlockAddr(addr, size, OM_DEBUG)
#define omTestAlignedBlockAddr(addr, size)                  omDebugAlignedBlockAddr(addr, size, OM_DEBUG)
#define omTestChunkAddr(addr)                               omDebugBlockAddr(addr, size, OM_DEBUG)
#define omTestAlignedChunkAddr(addr)                        omDebugAlignedBlockAddr(addr, size, OM_DEBUG)

#define omTestReportBinAddr(addr)                           omDebugReportBinAddr(addr, OM_DEBUG)
#define omTestReportBlockAddr(addr, size)                   omDebugReportBlockAddr(addr, size, OM_DEBUG)
#define omTestReportAlignedBlockAddr(addr, size)            omDebugReportAlignedBlockAddr(addr, size, OM_DEBUG)
#define omTestReportChunkAddr(addr)                         omDebugReportBlockAddr(addr, size, OM_DEBUG)
#define omTestReportAlignedChunkAddr(addr)                  omDebugReportAlignedBlockAddr(addr, size, OM_DEBUG)

#define omTestUsedAddrs()                                   omDebugUsedAddrs(OM_DEBUG)
#define omTestFreeAddrs()                                   omDebugFreeAddrs(OM_DEBUG)
#define omTestAllAddrs()                                    omDebugAllAddrs(OM_DEBUG)
#define omTestReportUsedAddrs()                             omDebugReportUsedAddrs(OM_DEBUG)
#define omTestReportFreeAddrs()                             omDebugReportFreeAddrs(OM_DEBUG)
#define omTestReportAllAddrs()                              omDebugReportAllAddrs(OM_DEBUG)

#elsif defined(OM_CHECK) && OM_CHECK != 0 
/*******************************************************************
 * Alloc/Free for Check
 *******************************************************************/

#define omTypeAllocBin(type, addr, bin)                     addr = (type) omCheckAllocBin(bin, OM_DEBUG)
#define omTypeAlloc0Bin(type, addr, bin)                    addr = (type) omCheckAlloc0Bin(bin, OM_DEBUG)
#define omAllocBin(bin)                                     omCheckAllocBin(bin, OM_DEBUG)
#define omAlloc0Bin(bin)                                    omCheckAlloc0Bin(bin, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omFreeBin(addr)                                     omCheckPseudoFreeBin(addr, OM_DEBUG)
#else
#define omFreeBin(addr)                                     omCheckFreeBin(addr, OM_DEBUG)
#endif

#define omTypeAllocBlock(type, addr, size)                  addr = (type) omCheckAllocBlock(size, OM_DEBUG)
#define omTypeAlloc0Block(type, addr, size)                 addr = (type) omCheckAlloc0Block(size, OM_DEBUG)
#define omAllocBlock(size)                                  omCheckAllocBlock(size, OM_DEBUG)
#define omAlloc0Block(size)                                 omCheckAlloc0Block(size, OM_DEBUG)
#define omTypeAllocAlignedBlock(type, addr, size)           addr = (type) omCheckAllocAlignedBlock(size, OM_DEBUG)
#define omTypeAlloc0AlignedBlock(type, addr, size)          addr = (type) omCheckAlloc0AlignedBlock(size, OM_DEBUG)
#define omAllocAlignedBlock(size)                           omCheckAllocAlignedBlock(size, OM_DEBUG)
#define omAlloc0AlignedBlock(size)                          omCheckAlloc0AlignedBlock(size, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocBlock(addr, old_size, new_size)            omCheckPseudoReallocBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0Block(addr, old_size, new_size)           omCheckPseudoRealloc0Block(addr, old_size, new_size, OM_DEBUG)
#define omReallocAlignedBlock(addr, old_size, new_size)     omCheckPseudoReallocAlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    omCheckPseudoRealloc0AlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omFreeBlock(addr, size)                             omCheckPseudoFreeBlock(addr, size, OM_DEBUG)
#else
#define omReallocBlock(addr, old_size, new_size)            omCheckReallocBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0Block(addr, old_size, new_size)           omCheckRealloc0Block(addr, old_size, new_size, OM_DEBUG)
#define omReallocAlignedBlock(addr, old_size, new_size)     omCheckReallocAlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    omCheckRealloc0AlignedBlock(addr, old_size, new_size, OM_DEBUG)
#define omFreeBlock(addr, size)                             omCheckFreeBlock(addr, size, OM_DEBUG)
#endif

#define omTypeAllocChunk(type, addr, size)                  addr = (type) omCheckAllocChunk(size, OM_DEBUG)
#define omTypeAlloc0Chunk(type, addr, size)                 addr = (type) omCheckAlloc0Chunk(size, OM_DEBUG)
#define omAllocChunk(size)                                  omCheckAllocChunk(size, OM_DEBUG)
#define omAlloc0Chunk(size)                                 omCheckAlloc0Chunk(size, OM_DEBUG)
#define omTypeAllocAlignedChunk(type, addr, size)           addr = (type) omCheckAllocAlignedChunk(size, OM_DEBUG)
#define omTypeAlloc0AlignedChunk(type, addr, size)          addr = (type) omCheckAlloc0AlignedChunk(size, OM_DEBUG)
#define omAllocAlignedChunk(size)                           omCheckAllocAlignedChunk(size, OM_DEBUG)
#define omAlloc0AlignedChunk(size)                          omCheckAlloc0AlignedChunk(size, OM_DEBUG)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocChunk(addr, new_size)                      omCheckPseudoReallocChunk(addr, new_size, OM_DEBUG)
#define omReallocAlignedChunk(addr, new_size)               omCheckPseudoReallocAlignedChunk(addr, new_size, OM_DEBUG)
#define omFreeChunk(addr)                                   omCheckPseudoFreeChunk(addr, OM_DEBUG)
#define omFreeAlignedChunk(addr)                            omCheckPseudoFreeAlignedChunk(addr, OM_DEBUG)
#else
#define omReallocChunk(addr, new_size)                      omCheckReallocChunk(addr, new_size, OM_DEBUG)
#define omReallocAlignedChunk(addr, new_size)               omCheckReallocAlignedChunk(addr, new_size, OM_DEBUG)
#define omFreeChunk(addr)                                   omCheckFreeChunk(addr, OM_DEBUG)
#define omFreeAlignedChunk(addr)                            omCheckFreeAlignedChunk(addr, OM_DEBUG)
#endif


#define omTestBinAddr(addr)                                 omCheckBinAddr(addr, OM_DEBUG)
#define omTestBlockAddr(addr, size)                         omCheckBlockAddr(addr, size, OM_DEBUG)
#define omTestAlignedBlockAddr(addr, size)                  omCheckAlignedBlockAddr(addr, size, OM_DEBUG)
#define omTestChunkAddr(addr)                               omCheckBlockAddr(addr, size, OM_DEBUG)
#define omTestAlignedChunkAddr(addr)                        omCheckAlignedBlockAddr(addr, size, OM_DEBUG)

#define omTestReportBinAddr(addr)                           omCheckReportBinAddr(addr, OM_DEBUG)
#define omTestReportBlockAddr(addr, size)                   omCheckReportBlockAddr(addr, size, OM_DEBUG)
#define omTestReportAlignedBlockAddr(addr, size)            omCheckReportAlignedBlockAddr(addr, size, OM_DEBUG)
#define omTestReportChunkAddr(addr)                         omCheckReportBlockAddr(addr, size, OM_DEBUG)
#define omTestReportAlignedChunkAddr(addr)                  omCheckReportAlignedBlockAddr(addr, size, OM_DEBUG)

#define omTestUsedAddrs()                                   omCheckUsedAddrs(OM_DEBUG)
#define omTestFreeAddrs()                                   omCheckFreeAddrs(OM_DEBUG)
#define omTestAllAddrs()                                    omCheckAllAddrs(OM_DEBUG)
#define omTestReportUsedAddrs()                             omCheckReportUsedAddrs(OM_DEBUG)
#define omTestReportFreeAddrs()                             omCheckReportFreeAddrs(OM_DEBUG)
#define omTestReportAllAddrs()                              omCheckReportAllAddrs(OM_DEBUG)


#else 
/*******************************************************************
 * Alloc/Free -- the real thing
 *******************************************************************/

#define omTypeAllocBin(type, addr, bin)                     __omTypeAllocBin(type, addr, bin)
#define omTypeAlloc0Bin(type, addr, bin)                    __omTypeAllocBin(type, addr, bin)
#define omAllocBin(bin)                                     _omAllocBin(bin)
#define omAlloc0Bin(bin)                                    _omAlloc0Bin(bin)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omFreeBin(addr)                                     _omPseudoFreeBin(addr)
#else
#define omFreeBin(addr)                                     __omFreeBin(addr)
#endif

#define omTypeAllocBlock(type, addr, size)                  __omTypeAllocBlock(type, addr, size)
#define omTypeAlloc0Block(type, addr, size)                 __omTypeAlloc0Block(type, addr, size)
#define omAllocBlock(size)                                  _omAllocBlock(size)
#define omAlloc0Block(size)                                 _omAlloc0Block(size)
#define omTypeAllocAlignedBlock(type, addr, size)           __omTypeAllocAlignedBlock(type, addr, size)
#define omTypeAlloc0AlignedBlock(type, addr, size)          __omTypeAlloc0AlignedBlock(type, addr, size)
#define omAllocAlignedBlock(size)                           _omAllocAlignedBlock(size)
#define omAlloc0AlignedBlock(size)                          _omAlloc0AlignedBlock(size)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocBlock(addr, old_size, new_size)            _omPseudoReallocBlock(addr, old_size, new_size)
#define omRealloc0Block(addr, old_size, new_size)           _omPseudoRealloc0Block(addr, old_size, new_size)
#define omReallocAlignedBlock(addr, old_size, new_size)     _omPseudoReallocAlignedBlock(addr, old_size, new_size)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    _omPseudoRealloc0AlignedBlock(addr, old_size, new_size)
#define omFreeBlock(addr, size)                             _omPseudoFreeBlock(addr, size)
#else
#define omReallocBlock(addr, old_size, new_size)            _omReallocBlock(addr, old_size, new_size)
#define omRealloc0Block(addr, old_size, new_size)           _omRealloc0Block(addr, old_size, new_size)
#define omReallocAlignedBlock(addr, old_size, new_size)     _omReallocAlignedBlock(addr, old_size, new_size)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    _omRealloc0AlignedBlock(addr, old_size, new_size)
#define omFreeBlock(addr, size)                             __FreeBlock(addr, size)
#endif

#define omTypeAllocChunk(type, addr, size)                  __omAllocChunk(type, addr, size)
#define omTypeAlloc0Chunk(type, addr, size)                 __omAlloc0Chunk(type, addr, size)
#define omAllocChunk(size)                                  _omAllocChunk(size)
#define omAlloc0Chunk(size)                                 _omAlloc0Chunk(size)
#define omTypeAllocAlignedChunk(type, addr, size)           __omTypeAllocAlignedChunk(type, addr, size)
#define omTypeAlloc0AlignedChunk(type, addr, size)          __omTypeAlloc0AlignedChunk(type, addr, size)
#define omAllocAlignedChunk(size)                           _omAllocAlignedChunk(size)
#define omAlloc0AlignedChunk(size)                          _omAllocAlignedChunk(size)
#if defined(OM_PSEUDO_FREE) && OM_PSEUDO_FREE != 0
#define omReallocChunk(addr, new_size)                      _omPseudoReallocChunk(addr, new_size)
#define omReallocAlignedChunk(addr, new_size)               _omPseudoReallocAlignedChunk(addr, new_size)
#define omFreeChunk(addr)                                   _omPseudoFreeChunk(addr)
#define omFreeAlignedChunk(addr)                            _omPseudoFreeAlignedChunk(addr)
#else
#define omReallocChunk(addr, new_size)                      _omReallocChunk(addr, new_size)
#define omReallocAlignedChunk(addr, new_size)               _omReallocAlignedChunk(addr, new_size)
#define omFreeChunk(addr)                                   _omFreeChunk(addr)
#define omFreeAlignedChunk(addr)                            _omFreeAlignedChunk(addr)
#endif


#define omTestBinAddr(addr)                                 (NULL)
#define omTestBlockAddr(addr, size)                         (NULL)
#define omTestAlignedBlockAddr(addr, size)                  (NULL)
#define omTestChunkAddr(addr)                               (NULL)
#define omTestAlignedChunkAddr(addr)                        (NULL)

#define omTestReportBinAddr(addr)                           (1)
#define omTestReportBlockAddr(addr, size)                   (1)
#define omTestReportAlignedBlockAddr(addr, size)            (1)
#define omTestReportChunkAddr(addr)                         (1)
#define omTestReportAlignedChunkAddr(addr)                  (1)

#define omTestUsedAddrs()                                   (NULL)
#define omTestFreeAddrs()                                   (NULL)
#define omTestAllAddrs()                                    (NULL)
#define omTestReportUsedAddrs()                             (1)
#define omTestReportFreeAddrs()                             (1)
#define omTestReportAllAddrs()                              (1)

#endif /* om Macro definitions */

/*******************************************************************
 *  
 *  Bin stuff
 *  
 *******************************************************************/
omBin omGetSpecBin(size_t size);
omBin omGetAlignedSpecBin(size_t size);
void  omUnGetSpecBin(omBin *bin);

unsigned long omGetNewStickyBinTag(omBin bin);
void omSetStickyBinTag(omBin bin, unsigned long sticky);
void omUnSetStickyBinTag(omBin bin, unsigned long sticky);
void omDeleteStickyBinTag(omBin bin, unsigned long sticky);

unsigned long omGetNewStickyAllBinTag();
void omSetStickyAllBinTag(omBin bin, unsigned long sticky);
void omUnSetStickyAllBinTag(omBin bin, unsigned long sticky);
void omDeleteStickyAllBinTag(omBin bin, unsigned long sticky);

#include <stdio.h>
extern void omPrintBinStats(FILE* fd);
extern void omPrintBinStat(FILE * fd, omBin bin);



#else /* OM_USE_EXTERNAL_MALLOC */

/*******************************************************************
 *  
 *  Alternative set of defines
 *  
 *******************************************************************/
typedef size_t omBin;
typedef size_t omSpecBin;

#define omTypeAllocBin(type, addr, bin)                     addr = (type) malloc(bin)
#define omTypeAlloc0Bin(type, addr, bin)                    do {addr = (type) malloc(bin); memset(addr, 0, bin);} while(0)
#define omAllocBin(bin)                                     malloc(bin)
#define omAlloc0Bin(bin)                                    omAlternativeMalloc0(bin)
#define omFreeBin(addr)                                     free(addr)

#define omTypeAllocBlock(type, addr, size)                  addr = (type) malloc(size)
#define omTypeAlloc0Block(type, addr, size)                 do {addr = (type) malloc(size); memset(addr, 0, size);} while(0)
#define omAllocBlock(size)                                  malloc(size)
#define omAlloc0Block(size)                                 omAlternativeMalloc0(size)
#define omTypeAllocAlignedBlock(type, addr, size)           addr = (type) malloc(size)
#define omTypeAlloc0AlignedBlock(type, addr, size)          do {addr = (type) malloc(size); memset(addr, 0, size);} while(0)
#define omAllocAlignedBlock(size)                           malloc(size)
#define omAlloc0AlignedBlock(size)                          omAlternativeMalloc0(size)
#define omReallocBlock(addr, old_size, new_size)            realloc(addr, new_size)
#define omRealloc0Block(addr, old_size, new_size)           omAlternativeRealloc0Block(addr, old_size, new_size)
#define omReallocAlignedBlock(addr, old_size, new_size)     realloc(addr, new_size)
#define omRealloc0AlignedBlock(addr, old_size, new_size)    omAlternativeRealloc0Block(addr, old_size, new_size)
#define omFreeBlock(addr, size)                             free(addr)

#define omTypeAllocChunk(type, addr, size)                  addr = (type) malloc(size)
#define omTypeAlloc0Chunk(type, addr, size)                 do {addr = (type) malloc(size); memset(addr, 0, size);} while(0)
#define omAllocChunk(size)                                  malloc(size)
#define omAlloc0Chunk(size)                                 omAlternativeMalloc0(size)
#define omTypeAllocAlignedChunk(type, addr, size)           addr = (type) malloc(size)
#define omTypeAlloc0AlignedChunk(type, addr, size)          do {addr = (type) malloc(size); memset(addr, 0, size);} while(0)
#define omAllocAlignedChunk(size)                           malloc(size)
#define omAlloc0AlignedChunk(size)                          omAlternativeMalloc0(size)
#define omReallocChunk(addr, new_size)                      realloc(addr, new_size)
#define omAlignedReallocChunk(addr, new_size)               realloc(addr, new_size)
#define omFreeChunk(addr)                                   free(addr)
#define omFreeAlignedChunk(addr)                            free(addr)


#define omTestBinAddr(addr)                                 (NULL)
#define omTestBlockAddr(addr, size)                         (NULL)
#define omTestAlignedBlockAddr(addr, size)                  (NULL)
#define omTestChunkAddr(addr)                               (NULL)
#define omTestAlignedChunkAddr(addr)                        (NULL)

#define omTestReportBinAddr(addr)                           (1)
#define omTestReportBlockAddr(addr, size)                   (1)
#define omTestReportAlignedBlockAddr(addr, size)            (1)
#define omTestReportChunkAddr(addr)                         (1)
#define omTestReportAlignedChunkAddr(addr)                  (1)

#define omTestUsedAddrs()                                   (NULL)
#define omTestFreeAddrs()                                   (NULL)
#define omTestAllAddrs()                                    (NULL)
#define omTestReportUsedAddrs()                             (1)
#define omTestReportFreeAddrs()                             (1)
#define omTestReportAllAddrs()                              (1)

#endif /* OM_USE_EXTERNAL_MALLOC */

#endif /* OM_ALLOC_H */
