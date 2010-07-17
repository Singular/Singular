/*******************************************************************
 *  File:    omAllocDecl.h
 *  Purpose: declaration of Alloc routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_ALLOC_DECL_H
#define OM_ALLOC_DECL_H

#if defined(OM_NDEBUG) && defined(OMALLOC_H)
#if (SIZEOF_LONG == 8)
#define OM_T_FREE1
#define OM_T_FREE3
#define OM_T_STR
#define OM_T_ALLOC
#define OM_T_REALLOC
#undef OM_NDEBUG
#endif
#endif


#if !defined(OMALLOC_C) && !defined(OM_NO_MALLOC_MACROS)
#define calloc      omcalloc
#define malloc      omalloc
#define free        omfree
#define realloc     omrealloc
#define freeSize    omfreeSize
#define reallocSize omreallocSize
#endif

#if (!defined(OM_EMULATE_OMALLOC) && !defined(OM_NDEBUG) && (defined(OM_CHECK) || (defined(OM_HAVE_TRACK) && defined(OM_TRACK)))) || defined(OM_T1)

/*******************************************************************
 *
 * Alloc/Free/Check for Debug
 *
 *******************************************************************/
#ifndef OM_CHECK
#define OM_CHECK    0
#endif
#ifndef OM_TRACK
#define OM_TRACK    0
#endif

#if OM_KEEP > 0
#define _OM_FKEEP OM_FKEEP
#else
#define _OM_FKEEP 0
#endif

#if !defined(OM_T_ALLOC)
#define omTypeAllocBin(type,addr,bin)           addr=(type)_omDebugAlloc(bin,OM_FBIN|_OM_FKEEP,OM_CTFL)
#define omTypeAlloc0Bin(type,addr,bin)          addr=(type)_omDebugAlloc(bin,OM_FBIN|OM_FZERO|_OM_FKEEP,OM_CTFL)
#define omAllocBin(bin)                         _omDebugAlloc(bin,OM_FBIN|_OM_FKEEP,OM_CTFL)
#define omAlloc0Bin(bin)                        _omDebugAlloc(bin,OM_FBIN|OM_FZERO|_OM_FKEEP,OM_CTFL)

#define omTypeAlloc(type,addr,size)             addr=(type)_omDebugAlloc((void*)(size),OM_FSIZE|_OM_FKEEP,OM_CTFL)
#define omTypeAlloc0(type,addr,size)            addr=(type)_omDebugAlloc((void*)(size),OM_FSIZE|OM_FZERO|_OM_FKEEP,OM_CTFL)
#define omAlloc(size)                           _omDebugAlloc((void*)(size),OM_FSIZE|_OM_FKEEP,OM_CTFL)
#define omAlloc0(size)                          _omDebugAlloc((void*)(size),OM_FSIZE|OM_FZERO|_OM_FKEEP,OM_CTFL)

#define omalloc(size)   _omDebugAlloc((void*)(size),OM_FSIZE|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omalloc0(size)  _omDebugAlloc((void*)(size),OM_FSIZE|OM_FZERO|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omcalloc(n,size)_omDebugAlloc((void*) ((size)*n),OM_FSIZE|OM_FZERO|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)

#else
#define omTypeAllocBin(type,addr,bin)           __omTypeAllocBin(type,addr,bin)
#define omTypeAlloc0Bin(type,addr,bin)          __omTypeAlloc0Bin(type,addr,bin)
#define omAllocBin(bin)                         _omAllocBin(bin)
#define omAlloc0Bin(bin)                        _omAlloc0Bin(bin)

#define omTypeAlloc(type,addr,size)             __omTypeAlloc(type,addr,size)
#define omTypeAlloc0(type,addr,size)            __omTypeAlloc0(type,addr,size)
#define omAlloc(size)                           _omAlloc(size)
#define omAlloc0(size)                          _omAlloc0(size)

#define omalloc(size)   _omalloc(size)
#define omalloc0(size)  _omalloc0(size)
#define omcalloc(n,size)_omalloc0(n*size)

#endif

#if !defined(OM_T_REALLOC)
#define omTypeReallocBin(o_addr,o_bin,type,addr,bin)            addr=(type)_omDebugRealloc(o_addr,o_bin,bin,OM_FBIN|_OM_FKEEP,OM_FBIN|_OM_FKEEP,OM_CTFL)
#define omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)           addr=(type)_omDebugRealloc(o_addr,o_bin,bin,OM_FBIN|_OM_FKEEP,OM_FBIN|OM_FZERO|_OM_FKEEP,OM_CTFL)
#define omReallocBin(o_addr,o_bin,bin)                          _omDebugRealloc(o_addr,o_bin,bin,OM_FBIN|_OM_FKEEP,OM_FBIN|_OM_FKEEP,OM_CTFL)
#define omRealloc0Bin(o_addr,o_bin,bin)                         _omDebugRealloc(o_addr,o_bin,bin,OM_FBIN|_OM_FKEEP,OM_FBIN|OM_FZERO|_OM_FKEEP,OM_CTFL)

#define omTypeReallocSize(o_addr,o_size,type,addr,size)         addr=(type)_omDebugRealloc(o_addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|_OM_FKEEP,OM_CTFL)
#define omTypeRealloc0Size(o_addr,o_size,type,addr,size)        addr=(type)_omDebugRealloc(o_addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FZERO|_OM_FKEEP,OM_CTFL)
#define omReallocSize(addr,o_size,size)                         _omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|_OM_FKEEP,OM_CTFL)
#define omRealloc0Size(addr,o_size,size)                        _omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FZERO|_OM_FKEEP,OM_CTFL)

#define omTypeRealloc(o_addr,type,addr,size)                    addr=(type)_omDebugRealloc(o_addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE|_OM_FKEEP,OM_CTFL)
#define omTypeRealloc0(o_addr,type,addr,size)                   addr=(type)_omDebugRealloc(o_addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE|OM_FZERO|_OM_FKEEP,OM_CTFL)
#define omRealloc(addr,size)                                    _omDebugRealloc(addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE,OM_CTFL)
#define omRealloc0(addr,size)                                   _omDebugRealloc(addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE|OM_FZERO,OM_CTFL)

#define omreallocSize(addr,o_size,size) _omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|OM_FSLOPPY|_OM_FKEEP,OM_FSIZE|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omrealloc0Size(addr,o_size,size)_omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|OM_FSLOPPY|_OM_FKEEP,OM_FSIZE|OM_FZERO|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omrealloc(addr,size)            _omDebugRealloc(addr,NULL,(void*)(size),OM_FSLOPPY|_OM_FKEEP,OM_FSIZE|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omrealloc0(addr,size)           _omDebugRealloc(addr,NULL,(void*)(size),OM_FSLOPPY|_OM_FKEEP,OM_FSIZE|OM_FZERO|OM_FSLOPPY|OM_FALIGN|_OM_FKEEP,OM_CTFL)

#else
#define omTypeReallocBin(o_addr,o_bin,type,addr,bin)            __omTypeReallocBin(o_addr,o_bin,type,addr,bin)
#define omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)           __omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)
#define omReallocBin(o_addr,o_bin,bin)                          _omReallocBin(o_addr,o_bin,bin)
#define omRealloc0Bin(o_addr,o_bin,bin)                         _omRealloc0Bin(o_addr,o_bin,bin)

#define omTypeReallocSize(o_addr,o_size,type,addr,size)         __omTypeReallocSize(o_addr,o_size,type,addr,size)
#define omTypeRealloc0Size(o_addr,o_size,type,addr,size)        __omTypeRealloc0Size(o_addr,o_size,type,addr,size)
#define omReallocSize(addr,o_size,size)                         _omReallocSize(addr,o_size,size)
#define omRealloc0Size(addr,o_size,size)                        _omRealloc0Size(addr,o_size,size)

#define omTypeRealloc(o_addr,type,addr,size)                    __omTypeRealloc(o_addr,type,addr,size)
#define omTypeRealloc0(o_addr,type,addr,size)                   __omTypeRealloc0(o_addr,type,addr,size)
#define omRealloc(addr,size)                                    _omRealloc(addr,size)
#define omRealloc0(addr,size)                                   _omRealloc0(addr,size)

#define omreallocSize(addr,o_size,size) _omreallocSize(addr,o_size,size)
#define omrealloc0Size(addr,o_size,size)_omrealloc0Size(addr,o_size,size)
#define omrealloc(addr,size)            _omrealloc(addr, size)
#define omrealloc0(addr,size)           _omrealloc0(addr, size)

#endif

#if !defined(OM_T_FREE1)
#define omFreeBinAddr(addr)     _omDebugFree(addr,NULL,OM_FBINADDR|_OM_FKEEP,OM_CFL)
#define omFreeBin(addr,bin)     _omDebugFree(addr,bin,OM_FBIN|_OM_FKEEP,OM_CFL)
#else
#define omFreeBinAddr(addr)     __omFreeBinAddr(addr)
#define omFreeBin(addr,bin)     __omFreeBinAddr(addr)
#endif

#if !defined(OM_T_FREE2)
#define omFreeSize(addr,size)   _omDebugFree(addr,(void*)(size),OM_FSIZE|_OM_FKEEP,OM_CFL)
#define omfreeSize(addr,size)   _omDebugFree(addr,(void*)(size),OM_FSIZE|OM_FSLOPPY|_OM_FKEEP,OM_CFL)
#else
#define omFreeSize(addr,size)   __omFreeSize(addr,size)
#define omfreeSize(addr,size)   do {if (addr && size) omFreeSize(addr, size);} while (0)
#endif

#if !defined(OM_T_FREE3)
#define omFree(addr)            _omDebugFree(addr,0,0,OM_CFL)
#define omfree(addr)            _omDebugFree(addr,NULL,OM_FSLOPPY|_OM_FKEEP,OM_CFL)
#else
#define omFree(addr)            __omFree(addr)
#define omfree(addr)            do {if (addr) omFree(addr);} while (0)
#endif

#if !defined(OM_T_STR)
#define omStrDup(s)                             _omDebugStrDup(s,OM_TFL)
#define omMemDup(addr)                          _omDebugMemDup(addr,_OM_FKEEP,OM_CTFL)
#else
#define omStrDup(s)         _omStrDup(s)
#define omMemDup(s)         _omMemDup(s)
#endif

#define omDebugBinAddr(addr)                 _omDebugAddr(addr,NULL,OM_FBINADDR,OM_CFL)
#define omDebugAddrBin(addr, bin)            _omDebugAddr(addr,bin,OM_FBIN,OM_CFL)
#define omDebugBinAddrSize(addr, size)       _omDebugAddr(addr,(void*)(size),OM_FBINADDR|OM_FSIZE,OM_CFL)
#define omDebugAddrSize(addr,size)           _omDebugAddr(addr,(void*)(size),OM_FSIZE,OM_CFL)
#define omDebugAddr(addr)                    _omDebugAddr(addr,NULL, 0, OM_CFL)
#define omdebugAddrSize(addr,size)           _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FSLOPPY,OM_CFL)
#define omdebugAddr(addr)                    _omDebugAddr(addr,NULL, OM_FSLOPPY, OM_CFL)
#define omDebugBin(bin)                      _omDebugBin(bin,OM_CFL)
#define omDebugMemory()                      _omDebugMemory(OM_CFL)
#define omDebugIf(cond, statement)           do { if (cond) {statement;}} while (0)

#if OM_CHECK > 0
#define omCheckBinAddr      omDebugBinAddr
#define omCheckAddrBin      omDebugAddrBin
#define omCheckBinAddrSize  omDebugBinAddrSize
#define omCheckAddrSize     omDebugAddrSize
#define omCheckAddr         omDebugAddr
#define omcheckAddrSize     omdebugAddrSize
#define omcheckAddr         omdebugAddr
#define omCheckBin          omDebugBin
#define omCheckMemory       omDebugMemory
#define omCheckIf           omDebugIf
#endif /* OM_CHECK > 0 */


#ifdef OM_ALIGNMENT_NEEDS_WORK

#define omTypeAllocAligned(type,addr,size)             addr=(type)_omDebugAlloc((void*)(size),OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omTypeAlloc0Aligned(type,addr,size)            addr=(type)_omDebugAlloc((void*)(size),OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omAllocAligned(size)                           _omDebugAlloc((void*)(size),OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omAlloc0Aligned(size)                          _omDebugAlloc((void*)(size),OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)

#define omTypeReallocAlignedSize(o_addr,o_size,type,addr,size)         addr=(type)_omDebugRealloc(o_addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omTypeRealloc0AlignedSize(o_addr,o_size,type,addr,size)        addr=(type)_omDebugRealloc(o_addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omReallocAlignedSize(addr,o_size,size)                         _omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omRealloc0AlignedSize(addr,o_size,size)                        _omDebugRealloc(addr,(void*)(o_size),(void*)(size),OM_FSIZE|_OM_FKEEP,OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)

#define omTypeReallocAligned(o_addr,type,addr,size)                    addr=(type)_omDebugRealloc(o_addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omTypeRealloc0Aligned(o_addr,type,addr,size)                   addr=(type)_omDebugRealloc(o_addr,NULL,(void*)(size),_OM_FKEEP,OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omReallocAligned(addr,size)                                    _omDebugRealloc(addr,NULL,(void*)(size),0,OM_FSIZE|OM_FALIGN|_OM_FKEEP,OM_CTFL)
#define omRealloc0Aligned(addr,size)                                   _omDebugRealloc(addr,NULL,(void*)(size),0,OM_FSIZE|OM_FZERO|OM_FALIGN|_OM_FKEEP,OM_CTFL)

#define omMemDupAligned(addr)                       _omDebugMemDup(addr,OM_FALIGN|_OM_FKEEP,OM_CTFL)

#define omDebugBinAddrAligned(addr)                 _omDebugAddr(addr,NULL,OM_FBINADDR|OM_FALIGN,OM_CFL)
#define omDebugAddrAlignedBin(addr, bin)            _omDebugAddr(addr,bin,OM_FBIN|OM_FALIGN,OM_CFL)
#define omDebugAddrAlignedSize(addr,size)           _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FALIGN,OM_CFL)
#define omDebugAddrAligned(addr)                    _omDebugAddr(addr,NULL, OM_FALIGN, OM_CFL)
#define omdebugAddrAlignedSize(addr,size)           _omDebugAddr(addr,(void*)(size),OM_FSIZE|OM_FSLOPPY|OM_FALIGN,OM_CFL)
#define omdebugAddrAligned(addr)                    _omDebugAddr(addr,NULL, OM_FSLOPPY|OM_FALIGN, OM_CFL)

#if OM_CHECK > 0
#define omCheckBinAddrAligned    omDebugBinAddrAligned
#define omCheckAddrAlignedBin    omDebugAddrAlignedBin
#define omCheckAddrAlignedSize   omDebugAddrAlignedSize
#define omCheckAddrAligned       omDebugAddrAligned
#define omcheckAddrAlignedSize   omdebugAddrAlignedSize
#define omcheckAddrAligned       omdebugAddrAligned
#endif
#endif /* OM_ALIGNMENT_NEEDS_WORK */

#elif !defined(OM_EMULATE_OMALLOC)
/*******************************************************************
 *
 * Alloc/Free -- the real thing
 *
 *******************************************************************/
#define omTypeAllocBin(type,addr,bin)           __omTypeAllocBin(type,addr,bin)
#define omTypeAlloc0Bin(type,addr,bin)          __omTypeAlloc0Bin(type,addr,bin)
#define omAllocBin(bin)                         _omAllocBin(bin)
#define omAlloc0Bin(bin)                        _omAlloc0Bin(bin)

#define omTypeAlloc(type,addr,size)             __omTypeAlloc(type,addr,size)
#define omTypeAlloc0(type,addr,size)            __omTypeAlloc0(type,addr,size)
#define omAlloc(size)                           _omAlloc(size)
#define omAlloc0(size)                          _omAlloc0(size)

#define omTypeReallocBin(o_addr,o_bin,type,addr,bin)            __omTypeReallocBin(o_addr,o_bin,type,addr,bin)
#define omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)           __omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)
#define omReallocBin(o_addr,o_bin,bin)                          _omReallocBin(o_addr,o_bin,bin)
#define omRealloc0Bin(o_addr,o_bin,bin)                         _omRealloc0Bin(o_addr,o_bin,bin)

#define omTypeReallocSize(o_addr,o_size,type,addr,size)         __omTypeReallocSize(o_addr,o_size,type,addr,size)
#define omTypeRealloc0Size(o_addr,o_size,type,addr,size)        __omTypeRealloc0Size(o_addr,o_size,type,addr,size)
#define omReallocSize(addr,o_size,size)                         _omReallocSize(addr,o_size,size)
#define omRealloc0Size(addr,o_size,size)                        _omRealloc0Size(addr,o_size,size)

#define omTypeRealloc(o_addr,type,addr,size)                    __omTypeRealloc(o_addr,type,addr,size)
#define omTypeRealloc0(o_addr,type,addr,size)                   __omTypeRealloc0(o_addr,type,addr,size)
#define omRealloc(addr,size)                                    _omRealloc(addr,size)
#define omRealloc0(addr,size)                                   _omRealloc0(addr,size)

#define omalloc(size)   _omalloc(size)
#define omalloc0(size)  _omalloc0(size)
#define omcalloc(n,size)_omalloc0(n*size)

#define omreallocSize(addr,o_size,size) _omreallocSize(addr,o_size,size)
#define omrealloc0Size(addr,o_size,size)_omrealloc0Size(addr,o_size,size)
#define omrealloc(addr,size)            _omrealloc(addr, size)
#define omrealloc0(addr,size)           _omrealloc0(addr, size)

#define omfreeSize(addr,size)   do {if (addr && size) omFreeSize(addr, size);} while (0)
#define omfree(addr)            do {if (addr) omFree(addr);} while (0)

#ifdef OM_ALIGNMENT_NEEDS_WORK
#define omTypeAllocAligned(type,addr,size)      __omTypeAllocAligned(type,addr,size)
#define omTypeAlloc0Aligned(type,addr,size)     __omTypeAlloc0Aligned(type,addr,size)
#define omAllocAligned(size)                    _omAllocAligned(size)
#define omAlloc0Aligned(size)                   _omAlloc0Aligned(size)

#define omTypeReallocAlignedSize(o_addr,o_size,type,addr,size)  __omTypeReallocAlignedSize(o_addr,o_size,type,addr,size)
#define omTypeRealloc0AlignedSize(o_addr,o_size,type,addr,size) __omTypeRealloc0AlignedSize(o_addr,o_size,type,addr,size)
#define omReallocAlignedSize(addr,o_size,size)                  _omReallocAlignedSize(addr,o_size,size)
#define omRealloc0AlignedSize(addr,o_size,size)                 _omRealloc0AlignedSize(addr,o_size,size)

#define omTypeReallocAligned(o_addr,type,addr,size)             __omTypeReallocAligned(o_addr,type,addr,size)
#define omTypeRealloc0Aligned(o_addr,type,addr,size)            __omTypeRealloc0Aligned(o_addr,type,addr,size)
#define omReallocAligned(addr,size)                             _omReallocAligned(addr,size)
#define omRealloc0Aligned(addr,size)                            _omRealloc0Aligned(addr,size)

#define omMemDupAligned(addr)                                   _omMemDupAligned(addr)
#endif  /* OM_ALIGNMENT_NEEDS_WORK */

#define omFreeBinAddr(addr)     __omFreeBinAddr(addr)
#define omFreeBin(addr,bin)     __omFreeBinAddr(addr)
#define omFreeSize(addr,size)   __omFreeSize(addr,size)
#define omFree(addr)            __omFree(addr)

#define omStrDup(s)         _omStrDup(s)
#define omMemDup(s)         _omMemDup(s)



#else /* OM_EMULATE_OMALLOC */
/*******************************************************************
 *
 * Emulation of omalloc's Alloc/Free interface
 *
 *******************************************************************/

#include <omalloc/omMalloc.h>

extern void* omEmulateCalloc(size_t size);
extern void* omEmulateRealloc0Size(void* o_addr, size_t o_size, size_t n_size);
extern void* omEmulateRealloc0(void* o_addr, size_t n_size);

#define omTypeAllocBin(type,addr,bin)           addr=(type) OM_MALLOC_MALLOC(bin->sizeW << LOG_SIZEOF_LONG)
#define omTypeAlloc0Bin(type,addr,bin)          addr=(type) omEmulateAlloc0(bin->sizeW << LOG_SIZEOF_LONG)
#define omAllocBin(bin)                         OM_MALLOC_MALLOC(bin->sizeW << LOG_SIZEOF_LONG)
#define omAlloc0Bin(bin)                        omEmulateAlloc0(bin->sizeW << LOG_SIZEOF_LONG)

#define omTypeAlloc(type,addr,size)             addr=(type) OM_MALLOC_MALLOC(size)
#define omTypeAlloc0(type,addr,size)            addr=(type) omEmulateAlloc0(size)
#define omAlloc(size)                           OM_MALLOC_MALLOC(size)
#define omAlloc0(size)                          omEmulateAlloc0(size)

#define omTypeReallocBin(o_addr,o_bin,type,addr,bin)            addr=(type)OM_MALLOC_REALLOC(o_addr,bin->sizeW << LOG_SIZEOF_LONG)
#define omTypeRealloc0Bin(o_addr,o_bin,type,addr,bin)           addr=(type)omEmulateRealloc0Size(o_addr,o_bin->sizeW << LOG_SIZEOF_LONG,bin->sizeW << LOG_SIZEOF_LONG)
#define omReallocBin(o_addr,o_bin,bin)                          OM_MALLOC_REALLOC(o_addr,bin->sizeW << LOG_SIZEOF_LONG)
#define omRealloc0Bin(o_addr,o_bin,bin)                         omEmulateRealloc0Size(o_addr,o_bin->sizeW << LOG_SIZEOF_LONG,bin->sizeW << LOG_SIZEOF_LONG)

#define omTypeReallocSize(o_addr,o_size,type,addr,size)         addr=(type)OM_MALLOC_REALLOC(o_addr,size)
#define omTypeRealloc0Size(o_addr,o_size,type,addr,size)        addr=(type)omEmulateRealloc0Size(o_addr,o_size,size)
#define omReallocSize(addr,o_size,size)                         OM_MALLOC_REALLOC(addr,size)
#define omRealloc0Size(addr,o_size,size)                        omEmulateRealloc0Size(addr,o_size,size)

#define omTypeRealloc(o_addr,type,addr,size)                    addr=(type)OM_MALLOC_REALLOC(o_addr,size)
#define omTypeRealloc0(o_addr,type,addr,size)                   addr=(type)omEmulateRealloc0(o_addr,size)
#define omRealloc(addr,size)                                    OM_MALLOC_REALLOC(addr,size)
#define omRealloc0(addr,size)                                   omEmulateRealloc0(addr,size)

#define omFreeBin(addr,bin)     OM_MALLOC_FREE(addr)
#define omFreeSize(addr,size)   OM_MALLOC_FREE(addr)
#define omFree(addr)            OM_MALLOC_FREE(addr)
#endif /* ! debug && ! the real thing */


/* define alignment stuff, if necessary */
#if !defined(omTypeAllocAligned)
#define omTypeAllocAligned      omTypeAlloc
#define omTypeAlloc0Aligned     omTypeAlloc0
#define omAllocAligned          omAlloc
#define omAlloc0Aligned         omAlloc0

#define omTypeReallocAlignedSize     omTypeReallocSize
#define omTypeRealloc0AlignedSize    omTypeRealloc0Size
#define omReallocAlignedSize         omReallocSize
#define omRealloc0AlignedSize        omRealloc0Size

#define omTypeReallocAligned         omTypeRealloc
#define omTypeRealloc0Aligned        omTypeRealloc0
#define omReallocAligned             omRealloc
#define omRealloc0Aligned            omRealloc0

#define omMemDupAligned     omMemDup
#endif /* !defined(omTypeAllocAligned) */

#if !defined(omDebugAddrAlignedBin)
#define omDebugBinAddrAligned   omDebugBinAddr
#define omDebugAddrAlignedBin   omDebugAddrBin
#define omDebugAddrAlignedSize  omDebugAddrSize
#define omDebugAddrAligned      omDebugAddr
#define omdebugAddrAlignedSize  omdebugAddrSize
#define omdebugAddrAligned      omdebugAddr
#endif /* !defined(omDebugAddrAlignedBin) */

#if !defined(omCheckAddrAlignedBin)
#define omCheckBinAddrAligned   omCheckBinAddr
#define omCheckAddrAlignedBin   omCheckAddrBin
#define omCheckAddrAlignedSize  omCheckAddrSize
#define omCheckAddrAligned      omCheckAddr
#define omcheckAddrAlignedSize  omcheckAddrSize
#define omcheckAddrAligned      omcheckAddr
#endif /* !defined(omCheckAddrAlignedBin) */

/* define debug stuff, if necessary */
#if !defined(omDebugAddrBin)
#define omDebugIf(cond, test)                    ((void) 0)
#define omDebugBinAddr(addr)                     ((void) 0)
#define omDebugAddrBin(addr,bin)                 ((void) 0)
#define omDebugBinAddrSize(addr,size)            ((void) 0)
#define omDebugAddrSize(addr,size)               ((void) 0)
#define omDebugAddr(addr)                        ((void) 0)
#define omdebugAddrSize(addr,size)               ((void) 0)
#define omdebugAddr(addr)                        ((void) 0)
#define omDebugBin(bin)                          ((void) 0)
#define omDebugMemory()                          ((void) 0)
#endif /* !defined(omDebugAddrBin) */

/* define check stuff, if necessary */
#if !defined(omCheckAddrBin)
#define omCheckIf(cond, test)                    ((void) 0)
#define omCheckBinAddr(addr)                     ((void) 0)
#define omCheckAddrBin(addr,bin)                 ((void) 0)
#define omCheckBinAddrSize(addr,size)            ((void) 0)
#define omCheckAddrSize(addr,size)               ((void) 0)
#define omCheckAddr(addr)                        ((void) 0)
#define omcheckAddrSize(addr,size)               ((void) 0)
#define omcheckAddr(addr)                        ((void) 0)
#define omCheckBin(bin)                          ((void) 0)
#define omCheckMemory()                          ((void) 0)
#endif /* !defined(omCheckAddrBin) */



#if !defined(OM_NDEBUG) && !defined(OM_EMULATE_MALLOC)
omError_t omTestAddrBin(void* addr, omBin bin, int check_level);
omError_t omTestBinAddr(void* addr, int check_level);
omError_t omTestBinAddrSize(void* addr, size_t size, int check_level);
omError_t omTestAddrSize(void* addr, size_t size, int check_level);
omError_t omTestAddr(void* addr, int check_level);
omError_t omtestAddrSize(void* addr, size_t size, int check_level);
omError_t omtestAddr(void* addr, int check_level);
omError_t omTestAddrAlignedBin(void* addr, omBin bin, int check_level);
omError_t omTestAddrAlignedSize(void* addr, size_t size, int check_level);
omError_t omTestAddrAligned(void* addr, int check_level);
omError_t omtestAddrAlignedSize(void* addr, size_t size, int check_level);
omError_t omtestAddrAligned(void* addr, int check_level);
omError_t omTestBin(omBin bin, int check_level);
omError_t omTestMemory(int check_level);
#define omTestIf(cond, statement)           do { if (cond) {statement;}} while (0)

#else

#define omTestIf(cond, test)                      ((void) 0)
#define omTestAddrBin(addr,bin,l)                 ((void) 0)
#define omTestBinAddr(addr,l)                     ((void) 0)
#define omTestBinAddrSize(addr,size,l)            ((void) 0)
#define omTestAddrSize(addr,size,l)               ((void) 0)
#define omTestAddr(addr,l)                        ((void) 0)
#define omtestAddrSize(addr,size,l)               ((void) 0)
#define omtestAddr(addr,l)                        ((void) 0)
#define omTestAddrAlignedBin(addr,bin,l)                 ((void) 0)
#define omTestAddrAlignedSize(addr,size,l)               ((void) 0)
#define omTestAddrAligned(addr,l)                        ((void) 0)
#define omtestAddrAlignedSize(addr,size,l)               ((void) 0)
#define omtestAddrAligned(addr,l)                        ((void) 0)
#define omTestBin(bin,l)                          ((void) 0)
#define omTestMemory(l)                           ((void) 0)

#endif

#endif /* OM_ALLOC_DECL_H */
