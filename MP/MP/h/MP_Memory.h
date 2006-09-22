/****************************************************************
 *
 *   HEADER FILE:  MP_Memory.h
 *
 ***************************************************************/


#ifndef _MP_Memory_h
#define _MP_Memory_h

#ifdef EXTERNAL_MALLOC_H
#include EXTERNAL_MALLOC_H
#else

EXTERN void* (*IMP_MemAllocFnc) _ANSI_ARGS_((size_t));
EXTERN void* (*IMP_MemReallocFnc) _ANSI_ARGS_((void *, size_t, size_t));
EXTERN void  (*IMP_MemFreeFnc) _ANSI_ARGS_((void *, size_t));

EXTERN void* (*IMP_RawMemAllocFnc) _ANSI_ARGS_((size_t));
EXTERN void* (*IMP_RawMemReallocFnc) _ANSI_ARGS_((void *, size_t));
EXTERN void  (*IMP_RawMemFreeFnc) _ANSI_ARGS_((void *));

#endif /* EXTERNAL_MALLOC_H */

#endif /* _MP_Memory_h */



