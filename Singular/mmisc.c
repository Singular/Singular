/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmisc.c,v 1.10 1999-09-20 18:03:50 obachman Exp $ */

/*
* ABSTRACT:
*/

#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmpage.h"
#include "febase.h"
#include "distrib.h"

#ifdef MTRACK
#include "mmbt.h"
#endif

static int mm_specIndex = 0;
static int mm_specSize = 0;

size_t mmSpecializeBlock( size_t size )
{
  if (mm_specIndex != mmGetIndex( size ))
  {
    int mm_new_specIndex = mmGetIndex( size );
    if (mm_new_specIndex<0)
    {
      WerrorS("too many ring variables...");
    }
    else
    {
      mm_specIndex = mm_new_specIndex;
      mm_specHeap = &(mm_theList[mm_specIndex]);
    }
    mm_specSize = mmGetSize(mm_specIndex);
  }
  return mm_specSize;
}

size_t mmGetSpecSize()
{
  return mm_specSize;
}


size_t mmSizeL( void* adr )
{
  if (adr!=NULL)
  {
    adr = (size_t*)adr-1;
    return *(size_t*)adr;
  }
  return 0;
}

int mmMemAlloc( void )
{
  return mm_bytesMalloc + mm_bytesValloc;
}
int mmMemUsed( void )
{
  int bytesfree = 0;
  int i;

  for (i=0; mmGetSize(i) < MAX_BLOCK_SIZE; i++)
    bytesfree += mmListLength(mm_theList[i].current)*mm_theList[i].size;
  return
    mm_bytesMalloc + mm_bytesValloc
    - (bytesfree + mmListLength(mm_theList[i].current)*mm_theList[i].size);
}

#ifdef HAVE_SBRK
#include <unistd.h>
int mmMemPhysical( void )
{
  unsigned long ns = (unsigned long) sbrk(0);
  return (ns >=  mm_SbrkInit ? ns - mm_SbrkInit : mm_SbrkInit - ns);
}
#endif


int mm_bytesMalloc=0;
static int mm_printMark=102400;

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

void mmCheckPrint( void )
{
  int mm_bytesAlloc = mm_bytesValloc + mm_bytesMalloc;

  if ( ABS(mm_bytesAlloc - mm_printMark)>(100*1024) )
  {
    int i=(mm_bytesAlloc+1023)/1024;
    fprintf( stdout, "[%dk]", i);
    fflush( stdout );
    mm_printMark=mm_bytesAlloc;
  }
}

void mmPrintStat()
{
#ifndef MAKE_DISTRIBUTION
  int i;
  long l,a;

#ifdef HAVE_SBRK
  printf("Physical:%dk ", (mmMemPhysical()+ 1023)/1024);
#endif
  printf("Alloc:%dk ", (mmMemAlloc() + 1023)/1024);
  printf("Used:%dk ", (mmMemUsed()+ 1023)/1024);
  printf("Malloc:%dk ", (mm_bytesMalloc+ 1023)/1024);
  printf("Valloc:%dk ", (mm_bytesValloc+ 1023)/1024);
  printf("Palloc:%d ", mmGetNumberOfAllocatedPages());
  printf("Pused:%d ", mmGetNumberOfUsedPages());
  printf("\n");

  i=-1;
  printf("Heap\tSize\tPages\tUsage\tFree\tUsed\n");
  do
  {
    i++;
    l = mmListLength(mm_theList[i].pages);
    a = mmListLength(mm_theList[i].current);
    printf("%d\t%ld\t%ld\t%d\t%ld\t%ld\n",
           i, mmGetHeapBlockSize(&mm_theList[i]), l,
           (l != 0 ? ((int) ((1.0 -
                              ((double) a)
                              /
                              ((double) l*(SIZE_OF_PAGE/mmGetHeapBlockSize(&mm_theList[i])))
                              )*100.0))
            : 0),
           a,
           l*(SIZE_OF_PAGE/mmGetHeapBlockSize(&mm_theList[i])) - a);
  }
  while (mmGetSize(i) < MAX_BLOCK_SIZE);
#endif
}

#ifdef MLIST
void mmTestList (int all)
{
  DBMCB * what=mm_theDBused.next;
  fprintf(stderr,"list of used blocks:\n");
  while (what!=NULL)
  {
    if ((all & MM_PRINT_ALL_ADDR) || what->init == 0)
    {
      fprintf( stderr, "%d bytes at %p in: %s:%d",
               (int)what->size, what, what->fname, what->lineno);
#ifdef MTRACK
      mmDBPrintStack(what, all);
#else
      fprintf( stderr, "\n");
#endif
    }
    what=what->next;
  }
}
#endif


/**********************************************************************
 *
 * Some operations on linked lists of memory
 *
 **********************************************************************/

void* mmRemoveFromList(void* list, void* element)
{
  void* nlist;
  void* olist;

  if (list == NULL) return NULL;

  nlist = *((void**) list);
  olist = list;

  if (list == element) return nlist;

  while (nlist != NULL && nlist != element)
  {
    list = nlist;
    nlist = *((void**) list);
  }

  if (nlist != NULL) *((void**) list) = *((void**) nlist);

  return olist;
}

int mmListLength(void* list)
{
  int l = 0;
  while (list != NULL)
  {
    l++;
    list = *((void**) list);
  }
  return l;
}

void* mmListLast(void* memList)
{
  if (memList == NULL) return NULL;

  while (*((void**) memList) != NULL)
    memList = *((void**) memList);

  return memList;
}

int mmIsAddrOnList(void* addr, void* list)
{
  if (addr == NULL)
    return (list == NULL);

  while (list != NULL)
  {
    if (addr == list) return 1;
    list = *((void**) list);
  }
  return 0;
}

void* mmListHasCycle(void* list)
{
  void* l1 = list;
  void* l2;

  int l = 0, i;

  while (l1 != NULL)
  {
    i = 0;
    l2 = list;
    while (l1 != l2)
    {
      i++;
      l2 = *((void**) l2);
    }
    if (i != l) return l1;
    l1 = *((void**) l1);
    l++;
  }
  return NULL;
}


int mmGListLength(void* list, int next)
{
  int l = 0;
  while (list != NULL)
  {
    l++;
    list = *((void**) list + next);
  }
  return l;
}

void* mmGListLast(void* memList, int next)
{
  if (memList == NULL) return NULL;

  while (*((void**) memList) != NULL)
    memList = *((void**) memList + next);

  return memList;
}

int mmIsAddrOnGList(void* addr, void* list, int next)
{
  if (addr == NULL)
    return (list == NULL);

  while (list != NULL)
  {
    if (addr == list) return 1;
    list = *((void**) list + next);
  }
  return 0;
}

void* mmGListHasCycle(void* list, int next)
{
  void* l1 = list;
  void* l2;

  int l = 0, i;

  while (l1 != NULL)
  {
    i = 0;
    l2 = list;
    while (l1 != l2)
    {
      i++;
      l2 = *((void**) (l2 + next));
    }
    if (i != l) return l1;
    l1 = *((void**) (l1 + next));
    l++;
  }
  return NULL;
}

/**********************************************************************
 *
 * Operations for deep profiles
 *
 **********************************************************************/

#ifdef DO_DEEP_PROFILE
void _memcpyW(long* p1, long* p2, long l)
{
  assume(l >= 0);

  while(l)
  {
    *p1++ = *p2++;
    l--;
  }
}

void _memaddW(long* p1, long* p2, long* p3, long l)
{
  assume(l >= 0);

  while (l)
  {
    *p1++ = *p2++ + *p3++;
    l--;
  }
}

void _memsetW(long* p1, long w, long l)
{
  assume(l >= 0);

  while (l)
  {
    *p1++ = w;
    l--;
  }
}

#endif /* DO_DEEP_PROFILE */

#ifdef sun
#ifdef __svr4__
void bzero(char *s,int n)
{
  memset(s,0,n);
}
#endif
#endif
