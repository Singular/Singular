/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Id: mminit.cc,v 1.2 1997-03-24 14:25:24 Singular Exp $";
/*
* ABSTRACT:
*/

#define _POSIX_SOURCE 1

#include <stdlib.h>
#include <string.h>
#include "mod2.h"
#include "mmemory.h"
#include "mmprivat.h"
#ifdef HAVE_GMP
extern "C" {
#ifdef macintosh
#include "b_gmp.h"
#else
#include "gmp.h"
#endif
}
#endif

#ifdef ALIGN_8
mcb mm_normList[MAXLIST]=
{ NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL};
;
mcb mm_tmpList[MAXLIST]=
{ NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL};
;
#else
mcb mm_normList[MAXLIST]=
{ NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL};
mcb mm_tmpList[MAXLIST]=
{ NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,
  NULL,NULL};
#endif

mcb *mm_theList=mm_normList;

#ifdef MPOLYPART
void * mm_maxAddr=NULL;
void * mm_minAddr=NULL;
mcb mm_spec=NULL;
void *mm_spec_part=NULL;
#endif

#ifdef MDEBUG

DBMCB mm_theDBused;
DBMCB mm_theDBfree;
DBMCB mm_tmpDBused;
DBMCB mm_tmpDBfree;
DBMCB mm_normDBused;
DBMCB mm_normDBfree;
void * mm_maxAddr=NULL;
void * mm_minAddr=NULL;

#endif /* MDEBUG */

status_t mm_status = MM_NORMAL;

int mm_bytesUsed=0;
int mm_bytesReal=0;
int mm_bytesTmp=0;
int mm_bytesNorm=0;

int mm_specIndex=0;
size_t mm_specSize = mm_mcbSizes[0];

int mm_printMark=102400;

void* operator new ( size_t size )
{
#ifdef MDEBUG
  return mmDBAlloc( size, "new",0);
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

#ifdef HAVE_GMP
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
#endif

int mmInit();
#ifndef macintosh
static int mmIsInitialized=mmInit();
#else
int mmIsInitialized=0;
#endif

int mmInit( void )
{
  int ii;
#ifdef MM_STAT
  for(ii=0; ii<=MAXLIST; ii++)
  {
    mm_active[ii]=0; mm_max[ii]=0;
  }
  mm_max[MAXLIST+1]=0;
#endif
#ifdef MDEBUG
  if ( ! mmIsInitialized ) {
    memset(&mm_theDBused,0,sizeof(mm_theDBused));
    memset(&mm_theDBfree,0,sizeof(mm_theDBfree));
    memset(&mm_tmpDBused,0,sizeof(mm_tmpDBused));
    memset(&mm_tmpDBfree,0,sizeof(mm_tmpDBfree));
    memset(&mm_normDBused,0,sizeof(mm_normDBused));
    memset(&mm_normDBfree,0,sizeof(mm_normDBfree));
    mmIsInitialized=1;
  }
#endif /* MDEBUG */
#ifdef HAVE_GMP
#ifndef MDEBUG
  mp_set_memory_functions(mmAllocBlock,mmReallocBlock,mmFreeBlock);
#else
  mp_set_memory_functions(mgAllocBlock,mgReallocBlock,mgFreeBlock);
#endif
#endif
  return 1;
}
