/* emacs edit mode for this file is -*- C -*- */
/* $Id: memutil.h,v 1.3 2004-09-23 16:51:48 Singular Exp $ */

#ifndef INCL_MEMUTIL_H
#define INCL_MEMUTIL_H

#define _POSIX_SOURCE 1

#include <config.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void* getBlock ( size_t size );

void freeBlock ( void * block, size_t size );

void* reallocBlock ( void * block, size_t oldsize, size_t newsize );

#ifdef __cplusplus
struct memman_new_delete
{
  inline void* operator new( size_t size ) { return getBlock( size ); }
  inline void* operator new []( size_t size ) { return getBlock( size ); }
  inline void operator delete( void* block ) { freeBlock( block, 0 ); }
  inline void operator delete []( void* block ) { freeBlock( block, 0 ); }
};
}
#endif

#endif /* ! INCL_MEMUTIL_H */
