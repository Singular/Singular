/****************************************************************
 *                                                                  
 *   FILE:  MP_Memory.c
 *   
 *   This is simply to initialize the memory functions.
 *                                                                
 ***************************************************************/

#include "MP.h"
#include <stdlib.h>

void mfree(void *mm, size_t sz)
{
  free(mm);
}

void* mrealloc(void *mm, size_t s1, size_t s2)
{
  return realloc(mm, s1);
}

void* (*IMP_MemAllocFnc)(size_t) = malloc;
void* (*IMP_MemReallocFnc)(void *, size_t, size_t) = mrealloc;
void  (*IMP_MemFreeFnc)(void *, size_t) = mfree;

void* (*IMP_RawMemAllocFnc) _ANSI_ARGS_((size_t)) = malloc;
void* (*IMP_RawMemReallocFnc) _ANSI_ARGS_((void *, size_t)) = realloc;
void  (*IMP_RawMemFreeFnc) _ANSI_ARGS_((void *)) = free;




