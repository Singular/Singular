/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmalloc.c,v 1.2 1997-04-02 15:07:28 Singular Exp $ */

/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "mmemory.h"
#include "mmprivat.h"

size_t mmSizeL( void* adr )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    return *(size_t*)adr;
  }
  return 0;
}

#ifndef MDEBUG

void * mmAlloc( size_t size )
{
  size_t thesize = size + sizeof( ADDRESS );
  size_t * dummy = (size_t*)mmAllocBlock( thesize );
  *dummy = thesize;
  return (void*)(dummy+1);
}

void mmFree( void* adr )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    mmFreeBlock( adr, *(size_t*)adr );
  }
}

void * mmRealloc( void* adr, size_t newsize )
{
  size_t *aadr=(size_t*)(adr)-1;
  size_t oldsize = *aadr;
  void* newadr = mmAlloc( newsize );
  memcpy( newadr, adr, min(oldsize-sizeof(ADDRESS),newsize) );
  mmFreeBlock( aadr,oldsize );
  return newadr;
}

#else /* MDEBUG */

void * mmDBAlloc( size_t size, char* fname, int lineno )
{
  size_t thesize = size + sizeof( ADDRESS );
  size_t * dummy = (size_t*)mmDBAllocBlock( thesize, fname, lineno );
  *dummy = thesize;
  return (void*)(dummy+1);
}

void mmDBFree( void* adr, char* fname, int lineno )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    mmDBFreeBlock( adr,*(size_t *)adr, fname, lineno );
  }
}

void * mmDBRealloc( void* adr, size_t newsize, char* fname, int lineno )
{
  size_t *aadr=(size_t*)(adr)-1;
  size_t oldsize = *aadr;
  void* newadr = mmDBAlloc( newsize, fname, lineno );
  memcpy( newadr, adr, min(oldsize-sizeof(ADDRESS), newsize) );
  mmDBFreeBlock( aadr, oldsize, fname, lineno );
  return newadr;
}

#endif /* MDEBUG */
