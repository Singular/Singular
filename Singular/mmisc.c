/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmisc.c,v 1.22 1999-11-15 17:20:25 obachman Exp $ */

/*
* ABSTRACT:
*/

#include "mod2.h"
#include "mmemory.h"
#include "mmprivate.h"
#include "mmpage.h"
#include "febase.h"
#include "distrib.h"
#include "tok.h"

#ifdef MTRACK
#include "mmbt.h"
#endif

static int mm_specSize = 0;
static memSpecHeap mm_SpecHeaps = NULL;
static memSpecHeap mm_TempHeaps = NULL;

memHeap mmGetSpecHeap( size_t size)
{
  int mm_new_specIndex = mmGetIndex( size );
#ifdef ALIGN_8
  if (size % 8 != 0) size = size + 8 - (size % 8);
#endif
  if (mm_new_specIndex < 0 || 
      ((mm_theList[mm_new_specIndex].size != RealSizeFromSize(size)) && 
       (SIZE_OF_HEAP_PAGE/RealSizeFromSize(size) 
        > 
        SIZE_OF_HEAP_PAGE/mm_theList[mm_new_specIndex].size)))
  {
    memSpecHeap s_heap;
      s_heap 
        = mmFindInSortedGList(mm_SpecHeaps, 
                              ((void*) &s_heap->next) - ((void*) s_heap), 
                              ((void*) &s_heap->size) - ((void*) s_heap), 
                              RealSizeFromSize(size));
    if (s_heap != NULL) 
    {
      (s_heap->ref)++;
      assume(s_heap->heap != NULL && s_heap->heap->size == s_heap->size);
      return s_heap->heap;
    }
    s_heap = (memSpecHeap) Alloc0SizeOf(ip_memSpecHeap);
    s_heap->ref = 1;
    s_heap->heap = mmCreateHeap(RealSizeFromSize(size));
    s_heap->size = s_heap->heap->size;
    mm_SpecHeaps = 
      mmSortedInsertInGList(mm_SpecHeaps, 
                            ((void*) &s_heap->next) - ((void*) s_heap), 
                            ((void*) &s_heap->size) - ((void*) s_heap), 
                            s_heap);
    return s_heap->heap;
  }
  else
  {
      return &(mm_theList[mm_new_specIndex]);
  }
}

void mmUnGetSpecHeap(memHeap *heap_p)
{
  memHeap heap = *heap_p;
  int mm_this_specIndex = mmGetIndex(SizeFromRealSize(heap->size));

/*  mmGarbageCollectHeap(heap, 0); */

  if (mm_this_specIndex < 0 || 
      ((mm_theList[mm_this_specIndex].size != heap->size) && 
       (SIZE_OF_HEAP_PAGE/heap->size) 
        > 
       (SIZE_OF_HEAP_PAGE/mm_theList[mm_this_specIndex].size)))
  {
    memSpecHeap s_heap 
      = mmFindInSortedGList(mm_SpecHeaps, 
                            ((void*) &s_heap->next) - ((void*) s_heap), 
                            ((void*) &s_heap->size) - ((void*) s_heap), 
                            heap->size);
    if (s_heap == NULL)
    {
#ifdef MDEBUG
      fprintf(stderr, "Warning: mmUngetSpecHeap: heap not previously gotten with mmGetSpecHeap\n");
#else
      feReportBug("SpecHeap memory managment");
#endif
      return;
    }
    (s_heap->ref)--;
    if (s_heap->ref == 0)
    {
      mm_SpecHeaps = 
        mmRemoveFromGList(mm_SpecHeaps,
                          ((void*) &s_heap->next) - ((void*) s_heap),
                          s_heap);
      mmDestroyHeap(&(s_heap->heap));
      FreeSizeOf(s_heap, ip_memSpecHeap);
    }
  }
  *heap_p = NULL;
}
  
memHeap mmGetTempHeap(size_t size)
{
  memSpecHeap s_heap = (memSpecHeap) Alloc0SizeOf(ip_memSpecHeap);
  s_heap->heap = mmCreateHeap(size);
  mm_TempHeaps = 
    mmSortedInsertInGList(mm_TempHeaps,
                          ((void*) &s_heap->next) - ((void*) s_heap), 
                          ((void*) &s_heap->size) - ((void*) s_heap), 
                          s_heap);
  return s_heap->heap;
}

#ifndef HEAP_DEBUG
void mmUnGetTempHeap(memHeap *heap_p) 
#else
void mmDebugUnGetTemHeap(memHeap *heap_p, char* file, int line)
#endif
{
  memHeap heap = *heap_p;
  memSpecHeap s_heap;
  assume(heap != NULL);
  s_heap = 
    mmFindInSortedGList(mm_TempHeaps, 
                        ((void*) &s_heap->next) - ((void*) s_heap), 
                        ((void*) &s_heap->size) - ((void*) s_heap), 
                        heap->size);
  if (s_heap == NULL)
  {
#ifdef MDEBUG
    fprintf(stderr, "Warning: mmUngetTempHeap: heap not previously gotten with mmGetTempHeap\n");
#else
    feReportBug("TempHeap memory managment");
#endif
    return;
  }
  mm_TempHeaps = 
    mmRemoveFromGList(mm_SpecHeaps,
                      ((void*) &s_heap->next) - ((void*) s_heap),
                      s_heap);
  mmDestroyHeap(&(s_heap->heap));
  FreeSizeOf(s_heap, ip_memSpecHeap);
}

#ifndef HAVE_AUTOMATIC_GC 
void mmGarbageCollectHeaps(int strict)
{
#ifdef NDEBUG
  int show_mem = BVERBOSE(V_SHOW_MEM);
  int i;
  int s_strict = strict & 1;
  memSpecHeap s_heap;
  s_heap = mm_SpecHeaps;
  
  if (show_mem)
  {
    fprintf(stdout, "[%dk]:{%dk}gc", 
            (mm_bytesValloc + mm_bytesMalloc + 1023)/1024,
            (mmMemUsed()+1023)/1024);
    fflush(stdout);
    verbose &= ~Sy_bit(V_SHOW_MEM);
  }
  
  while (s_heap != NULL)
  {
    mmGarbageCollectHeap(s_heap->heap, s_strict);
    s_heap = s_heap->next;
  }
  if (strict & 4)
  {
    s_heap = mm_TempHeaps;
    while (s_heap != NULL)
    {
      mmGarbageCollectHeap(s_heap->heap, s_strict);
      s_heap = s_heap->next;
    }
  }
  for (i=0; mmGetSize(i) <= MAX_BLOCK_SIZE; i++)
  {
    mmGarbageCollectHeap(&mm_theList[i], s_strict);
  }
  if (strict & 2) mmReleaseFreePages();
  if (show_mem)
  {
    fprintf(stdout, "[%dk]:{%dk}", 
            (mm_bytesValloc + mm_bytesMalloc + 1023)/1024,
            (mmMemUsed()+1023)/1024);
    fflush(stdout);
    verbose |= Sy_bit(V_SHOW_MEM);
  }
#endif
}
#endif
  
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
  memSpecHeap s_heap;

#ifndef HAVE_AUTOMATIC_GC
  for (i=0; mmGetSize(i) <= MAX_BLOCK_SIZE; i++)
    bytesfree += mmListLength(mm_theList[i].current)*mm_theList[i].size;

  s_heap = mm_SpecHeaps;
  while (s_heap != NULL)
  {
    bytesfree += mmListLength(s_heap->heap->current) * (s_heap->heap->size);
    s_heap = s_heap->next;
  }
  s_heap = mm_TempHeaps;
  while (s_heap != NULL)
  {
    bytesfree += mmListLength(s_heap->heap->current) * s_heap->heap->size;
    s_heap = s_heap->next;
  }
#endif
  return
    mm_bytesMalloc + mm_bytesValloc 
     - bytesfree - mmGetNumberOfFreePages()*SIZE_OF_PAGE;
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
int mm_bytesValloc = 0;
static int mm_printMark=102400;

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

void mmCheckPrint( void )
{
  int mm_bytesAlloc = mm_bytesValloc + mm_bytesMalloc;

#if 0
  if ( ABS(mm_bytesAlloc - mm_printMark)>(100*1024) )
  {
    int i=(mm_bytesAlloc+1023)/1024;
    fprintf( stdout, "[%dk]", i);
    fflush( stdout );
    mm_printMark=mm_bytesAlloc;
  }
#else
  if ( ABS(mm_bytesAlloc - mm_printMark)>(1000*1024) )
  {
    mmPrintStat();
    mm_printMark=mm_bytesAlloc;
  }
#endif
}

extern memHeap mm_specHeap;

extern int mmNumberOfAllocatedHeapPages(memHeap heap);
extern int mmNumberOfFreeHeapBlocks(memHeap heap);

static void mmPrintHeapStat(memHeap heap, int i)
{
  long l,a;
  if (i >= 0) printf("%d", i);
  else if (i== -1) printf("S");
  else if (i== -2) printf("T");
  printf("\t%ld", heap->size);
  a = mmNumberOfFreeHeapBlocks(heap);
  if (heap->size > MAX_HEAP_CHUNK_SIZE)
  {
#ifndef HAVE_AUTOMATIC_GC
    l = (int) heap->pages;
    printf("\t%ld\t%d\t%ld\t%ld",
           l, (l > 0 ? ((int) ((1.0 -
                                ((double) a)
                                /
                                ((double) l)
                                )*100.0))
               : 0), a, l-a);
#endif
  }
  else
  {
    l = mmNumberOfAllocatedHeapPages(heap);
    printf("\t%ld\t%d\t%ld\t%ld",
           l,
           (l != 0 ? ((int) ((1.0 -
                              ((double) a)
                              /
                              ((double) l*(SIZE_OF_HEAP_PAGE/heap->size))
                              )*100.0))
            : 0),
           a,
           l*(SIZE_OF_HEAP_PAGE/heap->size) - a);
  }
  if (heap == mm_specHeap) printf(" *");
  else if (mm_specHeap != NULL && mm_specHeap->size == heap->size)
    printf(" +");
  printf("\n");
  fflush(stdout);
}
      
void mmPrintStat()
{
#ifndef MAKE_DISTRIBUTION
  int i;
  long l,a;
  memSpecHeap s_heap;
  
  mmTestHeaps();
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

  printf("Heap\tSize\tPages\tUsage\tFree\tUsed\n");
  for (i=0; mmGetSize(i) <= MAX_BLOCK_SIZE; i++)
  {
    mmPrintHeapStat(&(mm_theList[i]), i);
  }
  s_heap = mm_SpecHeaps;
  while (s_heap != NULL)
  {
    mmPrintHeapStat(s_heap->heap, -1);
    s_heap = s_heap->next;
  }
  s_heap = mm_TempHeaps;
  while (s_heap != NULL)
  {
    mmPrintHeapStat(s_heap->heap, -2);
    s_heap = s_heap->next;
  }
#endif
}

#ifdef MLIST
void mmTestList (FILE *fd, int all)
{
  DBMCB * what=mm_theDBused.next;
  fprintf(fd,"list of used blocks:\n");
  while (what!=NULL)
  {
    if ((all & MM_PRINT_ALL_ADDR) || ! (what->flags & MM_INITFLAG))
    {
      fprintf( fd, "%d bytes at %p allocated in: %s:%d",
               (int)what->size, what, what->allocated_fname, what->allocated_lineno);
#ifdef MTRACK
      mmDBPrintStack(fd, what, all);
#else
      fprintf( fd, "\n");
#endif
    }
    what=what->next;
  }
}
#endif

#ifdef HEAP_DEBUG
int mmTestHeaps()
{
  int i = -1;

  do
  {
    i++;
    if (! mmCheckHeap(&mm_theList[i])) return 0;
  }
  while (mmGetSize(i) < MAX_BLOCK_SIZE);

  return 1;
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
    list = *((void**) (list + next));
  }
  return l;
}

void* mmGListLast(void* memList, int next)
{
  if (memList == NULL) return NULL;

  while (*((void**) memList) != NULL)
    memList = *((void**) (memList + next));

  return memList;
}

int mmIsAddrOnGList(void* addr, void* list, int next)
{
  if (addr == NULL)
    return (list == NULL);

  while (list != NULL)
  {
    if (addr == list) return 1;
    list = *((void**) (list + next));
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

void* mmSortedInsertInGList(void* list, int next, int int_field, void* element)
{
  int el_size = *((int *) ((void**)(element + int_field)));
  
  if (list == NULL || *((int *)((void**)(list + int_field))) > el_size)
  {
    *((void**)(element + next)) = list;
    return element;
  }
  else
  {
    void* prev = list;
    void* curr = *((void**)(list + next));
    
    while (curr != NULL && *((int *)((void**)(curr + int_field))) > el_size)
    {
      prev = curr;
      curr = *((void**)(curr + next));
    }
    assume(prev != NULL);
    *((void**)(prev+ next)) = element;
    *((void**)(element+ next)) = curr;
    return list;
  }
}

void* mmFindInSortedGList(void* list, int next, int int_field, int what)
{
  while (1)
  {
    if (list == NULL || *((int *)((void**) (list + int_field))) > what)
      return NULL;
    if (*((int *)((void**)(list + int_field))) == what) return list;
    list = *((void**) (list + next));
  }
}

void* mmRemoveFromGList(void* list, int next, void* element)
{
  void* nlist;
  void* olist;

  if (list == NULL) return NULL;

  nlist = *((void**) (list + next));
  olist = list;

  if (list == element) return nlist;

  while (nlist != NULL && nlist != element)
  {
    list = nlist;
    nlist = *((void**) (list + next));
  }

  if (nlist != NULL) *((void**) (list + next)) = *((void**) (nlist + next));

  return olist;
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
