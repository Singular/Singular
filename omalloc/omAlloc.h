/*******************************************************************
 *  File:    omAlloc.h
 *  Purpose: declaration of public routines for omalloc  
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAlloc.h,v 1.1.1.1 1999-11-18 17:45:52 obachman Exp $
 *******************************************************************/
#ifndef OM_ALLOC_H
#define OM_ALLOC_H

/*******************************************************************
 *  
 *  Alloc/Free 
 *  
 *******************************************************************/

void* omFuncAlloc(size_t size);
void* omFuncAlloc0(size_t size);
void* omFuncAlloc(size_t size);
void* omFuncAlloc0(size_t size);
void  omFuncFree(void* addr);

void* omFuncAllocBlock(size_t size);
void* omFuncAlloc0Block(size_t size);
void  omFuncFreeBlock(void* addr, size_t size);

void* omFuncAllocHeap(omHeap heap);
void* omFuncAlloc0Heap(omHeap heap);
void  omFuncFreeHeap(void* addr);


#ifndef OM_DEBUG

#define omAlloc(size)                   _omAlloc(size)
#define omAlloc0(size)                  _omAlloc0(size)
#define omAllocAligned(size)            _omAllocAligned(size)
#define omTypeAlloc(size)               _omTypeAlloc(addr, type, size)
#define omTypeAlloc0(size)              _omTypeAlloc0(addr, type, size)
#define omTypeAllocAligned(size)        _omTypeAllocAligned(addr, type, size)
#define omFree(addr)                    _omFree(addr)

#define omAllocBlock(size)              _omAllocBlock(size)
#define omAlloc0Block(size)             _omAlloc0Block(size)
#define omAllocAlignedBlock(size)       _omAllocAlignedBlock(size)
#define omTypeAllocBlock(size)          _omTypeAllocBlock(addr, type, size)
#define omTypeAlloc0Block(size)         _omTypeAlloc0Block(addr, type, size)
#define omTypeAllocAlignedBlock(size)   _omTypeAllocAlignedBlock(addr, type, size)
#define omFreeBlock(addr, size)         _omFreeBlock(addr, size)

#define omAllocHeap(heap)               _omAllocHeap(heap)
#define omAlloc0Heap(heap)              _omAlloc0Heap(heap)
#define omAllocAlignedHeap(heap)        _omAllocAligned(heap)
#define omTypeAllocHeap(size)           _omTypeAllocHeap(addr, type, size)
#define omTypeAlloc0Heap(size)          _omTypeAlloc0Heap(addr, type, size)
#define omTypeAllocAlignedHeap(size)    _omTypeAllocAlignedHeap(addr, type, size)
#define omFreeHeap(addr)                _omFreeHeap(addr)

#define omDebugAlloc(size, fn, l)                    _omAlloc(size)
#define omDebugAlloc0(size, fn, l)                   _omAlloc0(size)
#define omDebugAllocAligned(size, fn, l)             _omAllocAligned(size)
#define omDebugTypeAlloc(size, fn, l)                _omTypeAlloc(addr, type, size)
#define omDebugTypeAlloc0(size, fn, l)               _omTypeAlloc0(addr, type, size)
#define omDebugTypeAllocAligned(size, fn, l)         _omTypeAllocAligned(addr, type, size)
#define omDebugFree(addr, fn, l)                     _omFree(addr)

#define omDebugAllocBlock(size, fn, l)               _omAllocBlock(size)
#define omDebugAlloc0Block(size, fn, l)              _omAlloc0Block(size)
#define omDebugAllocAlignedBlock(size, fn, l)        _omAllocAlignedBlock(size)
#define omDebugTypeAllocBlock(size, fn, l)           _omTypeAllocBlock(addr, type, size)
#define omDebugTypeAlloc0Block(size, fn, l)          _omTypeAlloc0Block(addr, type, size)
#define omDebugTypeAllocAlignedBlock(size, fn, l)    _omTypeAllocAlignedBlock(addr, type, size)
#define omDebugFreeBlock(addr, size, fn, l)          _omFreeBlock(addr, size)

#define omDebugAllocHeap(heap, fn, l)                _omAllocHeap(heap)
#define omDebugAlloc0Heap(heap, fn, l)               _omAlloc0Heap(heap)
#define omDebugAllocAlignedHeap(heap, fn, l)         _omAllocAligned(heap)
#define omDebugTypeAllocHeap(size, fn, l)            _omTypeAllocHeap(addr, type, size)
#define omDebugTypeAlloc0Heap(size, fn, l)           _omTypeAlloc0Heap(addr, type, size)
#define omDebugTypeAllocAlignedHeap(size, fn, l)     _omTypeAllocAlignedHeap(addr, type, size)
#define omDebugFreeHeap(addr, fn, l)                 _omFreeHeap(addr)


#define omTrackAlloc(size)                   _omAlloc(size)
#define omTrackAlloc0(size)                  _omAlloc0(size)
#define omTrackAllocAligned(size)            _omAllocAligned(size)
#define omTypeTrackAlloc(size)               _omTypeAlloc(addr, type, size)
#define omTypeTrackAlloc0(size)              _omTypeAlloc0(addr, type, size)
#define omTypeTrackAllocAligned(size)        _omTypeAllocAligned(addr, type, size)
#define omTrackFree(addr)                    _omFree(addr)

#define omTrackAllocBlock(size)              _omAllocBlock(size)
#define omTrackAlloc0Block(size)             _omAlloc0Block(size)
#define omTrackAllocAlignedBlock(size)       _omAllocAlignedBlock(size)
#define omTypeTrackAllocBlock(size)          _omTypeAllocBlock(addr, type, size)
#define omTypeTrackAlloc0Block(size)         _omTypeAlloc0Block(addr, type, size)
#define omTypeTrackAllocAlignedBlock(size)   _omTypeAllocAlignedBlock(addr, type, size)
#define omTrackFreeBlock(addr, size)          _omFreeBlock(addr, size)

#define omTrackAllocHeap(heap)               _omAllocHeap(heap)
#define omTrackAlloc0Heap(heap)              _omAlloc0Heap(heap)
#define omTrackAllocAlignedHeap(heap)        _omAllocAligned(heap)
#define omTypeTrackAllocHeap(size)           _omTypeAllocHeap(addr, type, size)
#define omTypeTrackAlloc0Heap(size)          _omTypeAlloc0Heap(addr, type, size)
#define omTypeTrackAllocAlignedHeap(size)    _omTypeAllocAlignedHeap(addr, type, size)
#define omTrackFreeHeap(addr)                _omFreeHeap(addr)

#define omDebugTrackAlloc(size, fn, l)                    _omAlloc(size)
#define omDebugTrackAlloc0(size, fn, l)                   _omAlloc0(size)
#define omDebugTrackAllocAligned(size, fn, l)             _omAllocAligned(size)
#define omDebugTypeTrackAlloc(size, fn, l)                _omTypeAlloc(addr, type, size)
#define omDebugTypeTrackAlloc0(size, fn, l)               _omTypeAlloc0(addr, type, size)
#define omDebugTypeTrackAllocAligned(size, fn, l)         _omTypeAllocAligned(addr, type, size)
#define omDebugTrackFree(addr, fn, l)                     _omFree(addr)

#define omDebugTrackAllocBlock(size, fn, l)               _omAllocBlock(size)
#define omDebugTrackAlloc0Block(size, fn, l)              _omAlloc0Block(size)
#define omDebugTrackAllocAlignedBlock(size, fn, l)        _omAllocAlignedBlock(size)
#define omDebugTypeTrackAllocBlock(size, fn, l)           _omTypeAllocBlock(addr, type, size)
#define omDebugTypeTrackAlloc0Block(size, fn, l)          _omTypeAlloc0Block(addr, type, size)
#define omDebugTypeTrackAllocAlignedBlock(size, fn, l)    _omTypeAllocAlignedBlock(addr, type, size)
#define omDebugTrackFreeBlock(addr, size, fn, l)          _omFreeBlock(addr, size)

#define omDebugTrackAllocHeap(heap, fn, l)                _omAllocHeap(heap)
#define omDebugTrackAlloc0Heap(heap, fn, l)               _omAlloc0Heap(heap)
#define omDebugTrackAllocAlignedHeap(heap, fn, l)         _omAllocAligned(heap)
#define omDebugTypeTrackAllocHeap(size, fn, l)            _omTypeAllocHeap(addr, type, size)
#define omDebugTypeTrackAlloc0Heap(size, fn, l)           _omTypeAlloc0Heap(addr, type, size)
#define omDebugTypeTrackAllocAlignedHeap(size, fn, l)     _omTypeAllocAlignedHeap(addr, type, size)
#define omDebugTrackFreeHeap(addr, fn, l)                 _omFreeHeap(addr)

#else

#define omAlloc(size)                   omDebugAlloc(size, __FILE__, __LINE__)
#define omAlloc0(size)                  omDebugAlloc0(size, __FILE__, __LINE__)
#define omAllocAligned(size)            omDebugAllocAligned(size, __FILE__, __LINE__)
#define omTypeAlloc(size)               omDebugTypeAlloc(addr, type, size, __FILE__, __LINE__)
#define omTypeAlloc0(size)              omDebugTypeAlloc0(addr, type, size, __FILE__, __LINE__)
#define omTypeAllocAligned(size)        omDebugTypeAllocAligned(addr, type, size, __FILE__, __LINE__)
#define omFree(addr)                    omDebugFree(addr, __FILE__, __LINE__)

#define omAllocBlock(size)              omDebugAllocBlock(size, __FILE__, __LINE__)
#define omAlloc0Block(size)             omDebugAlloc0Block(size, __FILE__, __LINE__)
#define omAllocAlignedBlock(size)       omDebugAllocAlignedBlock(size, __FILE__, __LINE__)
#define omTypeAllocBlock(size)          omDebugTypeAllocBlock(addr, type, size, __FILE__, __LINE__)
#define omTypeAlloc0Block(size)         omDebugTypeAlloc0Block(addr, type, size, __FILE__, __LINE__)
#define omTypeAllocAlignedBlock(size)   omDebugTypeAllocAlignedBlock(addr, type, size, __FILE__, __LINE__)
#define omFreeBlock(addr, size)         omDebugFreeBlock(addr, size, __FILE__, __LINE__)

#define omAllocHeap(heap)               omDebugAllocHeap(heap, __FILE__, __LINE__)
#define omAlloc0Heap(heap)              omDebugAlloc0Heap(heap, __FILE__, __LINE__)
#define omAllocAlignedHeap(heap)        omDebugAllocAligned(heap, __FILE__, __LINE__)
#define omTypeAllocHeap(size)           omDebugTypeAllocHeap(addr, type, size, __FILE__, __LINE__)
#define omTypeAlloc0Heap(size)          omDebugTypeAlloc0Heap(addr, type, size, __FILE__, __LINE__)
#define omTypeAllocAlignedHeap(size)    omDebugTypeAllocAlignedHeap(addr, type, size, __FILE__, __LINE__)
#define omFreeHeap(addr)                omDebugFreeHeap(addr, __FILE__, __LINE__)

#define omTrackAlloc(size)                   omDebugTrackAlloc(size, __FILE__, __LINE__)
#define omTrackAlloc0(size)                  omDebugTrackAlloc0(size, __FILE__, __LINE__)
#define omTrackAllocAligned(size)            omDebugTrackAllocAligned(size, __FILE__, __LINE__)
#define omTypeTrackAlloc(size)               omDebugTypeTrackAlloc(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAlloc0(size)              omDebugTypeTrackAlloc0(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAllocAligned(size)        omDebugTypeTrackAllocAligned(addr, type, size, __FILE__, __LINE__)
#define omTrackFree(addr)                    omDebugFree(addr, __FILE__, __LINE__)

#define omTrackAllocBlock(size)              omDebugTrackAllocBlock(size, __FILE__, __LINE__)
#define omTrackAlloc0Block(size)             omDebugTrackAlloc0Block(size, __FILE__, __LINE__)
#define omTrackAllocAlignedBlock(size)       omDebugTrackAllocAlignedBlock(size, __FILE__, __LINE__)
#define omTypeTrackAllocBlock(size)          omDebugTypeTrackAllocBlock(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAlloc0Block(size)         omDebugTypeTrackAlloc0Block(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAllocAlignedBlock(size)   omDebugTypeTrackAllocAlignedBlock(addr, type, size, __FILE__, __LINE__)
#define omTrackFreeBlock(addr, size)         omDebugFreeBlock(addr, size, __FILE__, __LINE__)

#define omTrackAllocHeap(heap)               omDebugTrackAllocHeap(heap, __FILE__, __LINE__)
#define omTrackAlloc0Heap(heap)              omDebugTrackAlloc0Heap(heap, __FILE__, __LINE__)
#define omTrackAllocAlignedHeap(heap)        omDebugTrackAllocAligned(heap, __FILE__, __LINE__)
#define omTypeTrackAllocHeap(size)           omDebugTypeTrackAllocHeap(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAlloc0Heap(size)          omDebugTypeTrackAlloc0Heap(addr, type, size, __FILE__, __LINE__)
#define omTypeTrackAllocAlignedHeap(size)    omDebugTypeTrackAllocAlignedHeap(addr, type, size, __FILE__, __LINE__)
#define omTrackFreeHeap(addr)                omDebugFreeHeap(addr, __FILE__, __LINE__)


#endif /* OM_DEBUG */

#ifdef HAVE_CONFIG_H
#include "omConfig.h"
#endif

#include "omPrivate.h"

#endif /* OM_ALLOC_H */
