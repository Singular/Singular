/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mminit.cc,v 1.19 1999-11-17 13:35:34 hannes Exp $ */
/*
* ABSTRACT: init of memory management
*/

#define _POSIX_SOURCE 1

#include <stdlib.h>
#include <string.h>
#define __USE_MISC
#include <unistd.h>
#ifdef macintosh
#include <types.h>
#else
#include <sys/types.h>
#endif
#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
extern "C" { /* begin of "C" */
#include <gmp.h>
memHeap mm_specHeap = NULL;
} /* end of "C" */

void* operator new ( size_t size )
{
#ifdef MDEBUG
#if defined(MTRACK) && defined(linux) && defined(__i386__)
  extern int mm_no_mtrack;
  mm_no_mtrack = 1;
#endif
  void* addr;
  addr = mmDBAlloc( size, "new",0);
#if defined(MTRACK) && defined(linux) && defined(__i386__)
  mm_no_mtrack = 0;
#endif
  return addr;
#else
  return AllocL( size );
#endif
}

void operator delete ( void* block )
{
#ifdef MDEBUG
  mmDBFree( block, "delete",0);
#else
  FreeL( block );
#endif
}

#ifdef MDEBUG
void * mgAllocBlock( size_t t)
{
  return mmDBAllocBlock(t,"gmp",0);
}
void mgFreeBlock( void* a, size_t t)
{
  mmDBFreeBlock(a,t,"gmp",0);
}
void * mgReallocBlock( void* a, size_t t1, size_t t2)
{
  return mmDBReallocBlock(a,t1,t2,"gmp",0);
}
#endif

#ifdef HAVE_SBRK
#include <unistd.h>
unsigned long mm_SbrkInit = (unsigned long)sbrk(0);
#endif

static int mmIsInitialized=mmInit();

int mmInit( void )
{
  if(mmIsInitialized==0)
  {
#ifndef MDEBUG
    mp_set_memory_functions(mmAllocBlock,mmReallocBlock,mmFreeBlock);
#else
    mp_set_memory_functions(mgAllocBlock,mgReallocBlock,mgFreeBlock);
#endif
  }
  mmIsInitialized=1;
  return 1;
}
