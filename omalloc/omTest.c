#include <time.h>
#include <string.h>

#define CHECK_LEVEL   2
// #define MAX_CELLS     600000
#define MAX_CELLS     5000

#ifdef OM_TEST_MALLOC
#define OM_EMULATE_OMALLOC
#endif

#include "omAlloc.h"

#if defined(OM_TEST_MALLOC) || CHECK_LEVEL == 0
#include "omEmulate.h"
#endif

#ifdef OM_TEST_MALLOC
#define omPrintBinStats(fd) ((void)0)
#endif

struct omMemCell_s
{
  void* addr;
  omBin bin;
  int spec;
};

typedef struct omMemCell_s omMemCell_t;
typedef omMemCell_t* omMemCell;


#if CHECK_LEVEL > 2
#define myprintf(format, args...) \
  printf(format, ## args)
#define myfflush(what) fflush(what)
#else
#define myprintf(format, args...) ((void) 0)
#define myfflush(what)            ((void) 0)
#endif

#define GET_SIZE(spec)      (spec & ((1 << 14) -1))
#define SET_SIZE(spec, size) spec = ((spec & ~((1 << 14) -1)) | (size))
#define IS_ALIGNED(spec)    (spec & (1 << 15))
// #define IS_ALIGNED(spec) 0
#define IS_ZERO(spec)       (spec & (1 << 16))
#define IS_FREE_SPEC(spec)  (spec & (1 << 17))
#define IS_MACRO(spec)      (!(spec & (1 << 18))  && !(spec & (1 << 19)))
#define IS_INLINE(spec)     ((spec & (1 << 18))  && !(spec & (1 << 19)))
#define IS_FUNC(spec)       (!(spec & (1 << 18))  && (spec & (1 << 19)))
#define IS_CHECK(spec)      ((spec & (1 << 18))  && (spec & (1 << 19)))
#define IS_FREE_MACRO(spec)      (!(spec & (1 << 20))  && !(spec & (1 << 21)))
#define IS_FREE_INLINE(spec)     ((spec & (1 << 20))  && !(spec & (1 << 21)))
#define IS_FREE_FUNC(spec)       (!(spec & (1 << 20))  && (spec & (1 << 21)))
#define IS_FREE_CHECK(spec)      ((spec & (1 << 20))  && (spec & (1 << 21)))
#define DO_FREE(spec)            (!(spec & (1 << 23))  && !(spec & (1 << 24))) 
#define DO_REALLOC(spec)         ((spec & (1 << 23))  && (spec & (1 << 24)))
#define IS_BIN(spec)        (spec & (1 << 26))
#ifndef OM_TEST_MALLOC
#define IS_SPEC_BIN(spec)   (spec & (1 << 25))
#else
#define IS_SPEC_BIN(spec)   0
#endif

#define SPEC_MAX  ((1 << 27) -1)
#define SIZE_MAX  ((1 << 15) -1)
#define RANGE_MIN (1 << 10)
#define RANGE_MAX (1 << 14)

#define PAGES_PER_REGION 128

void omTestCheck(omMemCell cell)
{
  int i, j;
  size_t size = GET_SIZE(cell->spec);
  if (cell->bin != NULL)
  {
    if (IS_ALIGNED(cell->spec))
    {
      if (omCheckAlignedBinAddrBin(cell->addr, cell->bin, CHECK_LEVEL)) return;
    }
    else
    {
      if (omCheckBinAddrBin(cell->addr, cell->bin, CHECK_LEVEL)) return;
    }
  }
  else
  {
    if (IS_ALIGNED(cell->spec))
    {
      if (omCheckAlignedAddrSize(cell->addr, size, CHECK_LEVEL)) return;
    }
    else
    {
      if (omCheckAddrSize(cell->addr, size, CHECK_LEVEL)) return;
    }
    
  }
  if (omCheckAddr(cell->addr, CHECK_LEVEL - 1)) return;
  if (IS_ZERO(cell->spec)) j = 0;
  else j = 255;
  for (i=0; i< size; i++)
  {
    if (((unsigned char*) cell->addr)[i] != j) 
    {
      omReportError(omError_Unknown, "byte %d modified: is %d should be %d", i, ((unsigned char*) cell->addr)[i], j);
      return;
    }
  }
}

void omTestAlloc(omMemCell cell, int spec)
{
  int size = GET_SIZE(spec);
  void* addr;
  omBin bin = NULL;

  if (IS_BIN(spec) && (size <= OM_MAX_BLOCK_SIZE || IS_SPEC_BIN(spec)))
  {
    if (IS_SPEC_BIN(spec))
    {
      
      if (IS_ALIGNED(spec))
        bin = omGetAlignedSpecBin(size);
      else
        bin = omGetSpecBin(size); 
    }
    else
    {
      if (IS_ALIGNED(spec))
        bin = omSmallSize2AlignedBin(size);
      else
        bin = omSmallSize2Bin(size);
    }
    
    if (IS_MACRO(spec))
    {
      if (IS_ZERO(spec))
        omTypeAlloc0Bin(void*, addr, bin);
      else
        omTypeAllocBin(void*, addr, bin);
    }
    else if (IS_INLINE(spec))
    {
      if (IS_ZERO(spec))
        addr = omAlloc0Bin(bin);
      else
        addr = omAllocBin(bin);
    }
    else if (IS_FUNC(spec))
    {
      if (IS_ZERO(spec))
        addr = omFuncAlloc0Bin(bin);
      else
        addr = omFuncAllocBin(bin);
    }
    else
    {
      if (IS_ZERO(spec))
        addr = omCheckAlloc0Bin(bin, CHECK_LEVEL);
      else
        addr = omCheckAllocBin(bin, CHECK_LEVEL);
    }
  }
  else
  {
    if (IS_MACRO(spec))
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
          omTypeAlloc0Aligned(void*, addr, size);
        else
          omTypeAlloc0(void*, addr, size);
      }
      else
      {
        if (IS_ALIGNED(spec))
          omTypeAllocAligned(void*, addr, size);
        else
          omTypeAlloc(void*, addr, size);
      }
    }
    else if (IS_INLINE(spec))
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
          addr = omAlloc0Aligned(size);
        else
          addr = omAlloc0(size);
      }
      else
      {
        if (IS_ALIGNED(spec))
          addr = omAllocAligned(size);
        else
          addr = omAlloc(size);
      }
    }
    else if (IS_FUNC(spec))
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
          addr = omFuncAlloc0Aligned(size);
        else
          addr = omFuncAlloc0(size);
      }
      else
      {
        if (IS_ALIGNED(spec))
          addr = omFuncAllocAligned(size);
        else
          addr = omFuncAlloc(size);
      }
    }
    else
    {
      if (IS_ZERO(spec))
      {
        if (IS_ALIGNED(spec))
          addr = omCheckAlloc0Aligned(size, CHECK_LEVEL);
        else
          addr = omCheckAlloc0(size, CHECK_LEVEL);
      }
      else
      {
        if (IS_ALIGNED(spec))
          addr = omCheckAllocAligned(size, CHECK_LEVEL);
        else
          addr = omCheckAlloc(size, CHECK_LEVEL);
      }
    }
  }
  if (!IS_ZERO(spec)) memset(addr, 255, size);
  cell->addr = addr;
  cell->bin = bin;
  cell->spec = spec;
  omTestCheck(cell);
}

void omTestFree(omMemCell cell)
{
  void* addr = cell->addr;
  int spec = cell->spec;
  omBin bin = cell->bin;
  size_t size = GET_SIZE(spec);

  omTestCheck(cell);
  if (IS_FREE_SPEC(spec))
  {
    if (bin != NULL)
    {
      if (IS_FREE_MACRO(spec))
        omFreeBin(addr, bin);
      else if (IS_FREE_INLINE(spec))
        omFreeBin(addr, bin);
      else if (IS_FREE_FUNC(spec))
        omFuncFreeBin(addr, bin);
      else
        omCheckFreeBin(addr, bin, CHECK_LEVEL);
    }
    else
    {
      if (IS_FREE_MACRO(spec))
        omFreeSize(addr, size);
      else if (IS_FREE_INLINE(spec))
        omFreeSize(addr, size);
      else if (IS_FREE_FUNC(spec))
        omFuncFreeSize(addr, size);
      else
        omCheckFreeSize(addr, size, CHECK_LEVEL);
    }
  }
  else
  {
    if (IS_FREE_MACRO(spec))
      omFree(addr);
    else if (IS_FREE_INLINE(spec))
      omFree(addr);
    else if (IS_FREE_FUNC(spec))
      omFuncFree(addr);
    else
      omCheckFree(addr, CHECK_LEVEL);
  }
  if (bin != NULL && IS_SPEC_BIN(spec))
    omUnGetSpecBin(&bin);
  cell->addr = NULL;
  cell->spec = 0;
  cell->bin = NULL;
}

void omTestRealloc(omMemCell cell, int new_spec)
{
  void* old_addr = cell->addr;
  int old_spec = cell->spec;
  omBin old_bin = cell->bin;
  size_t old_size = GET_SIZE(old_spec);
  void* new_addr;
  omBin new_bin = NULL;
  size_t new_size = GET_SIZE(new_spec);

  omTestCheck(cell);
  omCheckAddrSize(old_addr, old_size, CHECK_LEVEL);
  if (IS_FREE_SPEC(old_spec))
  {
    if (old_bin != NULL && IS_BIN(new_spec) && ((new_size <= OM_MAX_BLOCK_SIZE) || IS_SPEC_BIN(new_spec)))
    {
      if (IS_SPEC_BIN(new_spec))
      {
      
        if (IS_ALIGNED(new_spec))
          new_bin = omGetAlignedSpecBin(new_size);
        else
          new_bin = omGetSpecBin(new_size); 
      }
      else
      {
        if (IS_ALIGNED(new_spec))
          new_bin = omSmallSize2AlignedBin(new_size);
        else
          new_bin = omSmallSize2Bin(new_size);
      }
        
      if (IS_MACRO(new_spec))
      {
        if (IS_ZERO(new_spec)) omTypeRealloc0Bin(old_addr, old_bin, void*, new_addr, new_bin);
        else omTypeReallocBin(old_addr, old_bin, void*, new_addr, new_bin);
      }
      else if (IS_INLINE(new_spec))
      {
        if (IS_ZERO(new_spec)) new_addr = omRealloc0Bin(old_addr, old_bin, new_bin);
        else new_addr = omReallocBin(old_addr, old_bin, new_bin);
      }
      else if (IS_FUNC(new_spec))
      {
        if (IS_ZERO(new_spec)) new_addr = omFuncRealloc0Bin(old_addr, old_bin, new_bin);
        else new_addr = omFuncReallocBin(old_addr, old_bin, new_bin);
      }
      else 
      {
        if (IS_ZERO(new_spec)) new_addr = omCheckRealloc0Bin(old_addr, old_bin, new_bin, CHECK_LEVEL);
        else new_addr = omCheckReallocBin(old_addr, old_bin, new_bin, CHECK_LEVEL);
      }
    }
    else
    {
      if (IS_MACRO(new_spec))
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec)) omTypeRealloc0AlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else  omTypeRealloc0Size(old_addr, old_size, void*, new_addr, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec))  omTypeReallocAlignedSize(old_addr, old_size, void*, new_addr, new_size);
          else  omTypeReallocSize(old_addr, old_size, void*, new_addr, new_size);
        }
      }
      else if (IS_INLINE(new_spec))
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec)) new_addr = omRealloc0AlignedSize(old_addr, old_size, new_size);
          else  new_addr = omRealloc0Size(old_addr, old_size, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec)) new_addr = omReallocAlignedSize(old_addr, old_size, new_size);
          else  new_addr = omReallocSize(old_addr, old_size, new_size);
        } 
      }
      else if (IS_FUNC(new_spec))
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec)) new_addr = omFuncRealloc0AlignedSize(old_addr, old_size, new_size);
          else  new_addr = omFuncRealloc0Size(old_addr, old_size, new_size);
        }
        else
        {
          if (IS_ALIGNED(new_spec)) new_addr = omFuncReallocAlignedSize(old_addr, old_size, new_size);
          else   new_addr = omFuncReallocSize(old_addr, old_size, new_size); 
        }
      }
      else 
      {
        if (IS_ZERO(new_spec))
        {
          if (IS_ALIGNED(new_spec))  new_addr = omCheckRealloc0AlignedSize(old_addr, old_size, new_size, CHECK_LEVEL);
          else  new_addr = omCheckRealloc0Size(old_addr, old_size, new_size, CHECK_LEVEL);
        }
        else
        {
          if (IS_ALIGNED(new_spec)) new_addr = omCheckReallocAlignedSize(old_addr, old_size, new_size, CHECK_LEVEL);
          else   new_addr = omCheckReallocSize(old_addr, old_size, new_size, CHECK_LEVEL);
        }
      }
    }
  }
  else
  {
    if (IS_MACRO(new_spec))
    {
      if (IS_ZERO(new_spec))
      {
        if (IS_ALIGNED(new_spec)) omTypeRealloc0Aligned(old_addr, void*, new_addr, new_size);
        else  omTypeRealloc0(old_addr, void*, new_addr, new_size);
      }
      else
      {
        if (IS_ALIGNED(new_spec))  omTypeReallocAligned(old_addr, void*, new_addr, new_size);
        else  omTypeRealloc(old_addr, void*, new_addr, new_size);
      }
    }
    else if (IS_INLINE(new_spec))
    {
      if (IS_ZERO(new_spec))
      {
        if (IS_ALIGNED(new_spec)) new_addr = omRealloc0Aligned(old_addr, new_size);
        else  new_addr = omRealloc0(old_addr, new_size);
      }
      else
      {
        if (IS_ALIGNED(new_spec)) new_addr = omReallocAligned(old_addr, new_size);
        else  new_addr = omRealloc(old_addr, new_size);
      } 
    }
    else if (IS_FUNC(new_spec))
    {
      if (IS_ZERO(new_spec))
      {
        if (IS_ALIGNED(new_spec)) new_addr = omFuncRealloc0Aligned(old_addr, new_size);
        else  new_addr = omFuncRealloc0(old_addr, new_size);
      }
      else
      {
        if (IS_ALIGNED(new_spec)) new_addr = omFuncReallocAligned(old_addr, new_size);
        else   new_addr = omFuncRealloc(old_addr, new_size); 
      }
    }
    else 
    {
      if (IS_ZERO(new_spec))
      {
        if (IS_ALIGNED(new_spec))  new_addr = omCheckRealloc0Aligned(old_addr, new_size, CHECK_LEVEL);
        else  new_addr = omCheckRealloc0(old_addr, new_size, CHECK_LEVEL);
      }
      else
      {
        if (IS_ALIGNED(new_spec)) new_addr = omCheckReallocAligned(old_addr, new_size, CHECK_LEVEL);
        else   new_addr = omCheckRealloc(old_addr, new_size, CHECK_LEVEL);
      }
    }
  }
  if (! IS_ZERO(new_spec)) memset(new_addr, 255, new_size);
  else memset(new_addr, 0, new_size);
  if (old_bin != NULL) omUnGetSpecBin(&old_bin);
  cell->addr = new_addr;
  cell->bin = new_bin;
  cell->spec = new_spec;
  omTestCheck(cell);
}
 

int size_range = RANGE_MIN;
int size_range_number = RANGE_MAX / RANGE_MIN;

int MyRandSpec()
{
  int spec = 1 + (int) ( ((double) SPEC_MAX)* rand()/(RAND_MAX + 1.0));
  
  if (! size_range_number)
  {
    size_range = size_range << 1;
    if (size_range > RANGE_MAX) size_range = RANGE_MIN;
    size_range_number = RANGE_MAX / size_range;
  }
  SET_SIZE(spec, GET_SIZE(spec) & (size_range -1));
  size_range_number--;
  if (GET_SIZE(spec) == 0) spec++;
  return spec;
}

int main(int argc, char* argv[])
{
  int i=0;
  int spec, j;
  omMemCell_t cells[MAX_CELLS];
  int seed = time(NULL);
  int n = 1;
  int n_cells = MAX_CELLS;
  int decr = 10;

  omInitTrack(argv[0]);
  omInitInfo();
  om_Opts.PagesPerRegion = PAGES_PER_REGION;
  
  if (argc > 1) sscanf(argv[1], "%d", &seed);
  srand(seed);

  if (argc > 2) sscanf(argv[2], "%d", &n);
  if (argc > 3) sscanf(argv[2], "%d", &decr);
  
  printf("seed == %d\n", seed);
  fflush(stdout);
  while (1)
  {
    if (i == n_cells)
    {
      i = 0;
      printf("Cells: %d\n", n_cells);
      omPrintStats(stdout);
      omPrintInfo(stdout);
      omPrintBinStats(stdout);
      printf("\n");
      fflush(stdout);
      while (i< n_cells)
      {
        omTestFree(&cells[i]);
        i++;
      }
      omPrintStats(stdout);
      omPrintInfo(stdout);
      omPrintBinStats(stdout);
      i=0;
      n--;
      if (n <= 0 || n_cells <= 100)
      {
        exit(0);
      }
      else
      {
        n_cells = n_cells / decr;
      }
    }
    spec = MyRandSpec();
    myprintf("%d:%d:%d", i, spec, GET_SIZE(spec));
    myfflush(stdout);
    if (DO_FREE(spec))
    {
      myprintf(" FREE");
      if (i != 0) 
      {
        j = spec % i;
        myprintf(" %d ", j);
        myfflush(stdout);
        omTestFree(&cells[j]);
        omTestAlloc(&cells[j], spec);
      }
    }
    else if (DO_REALLOC(spec))
    {
      if (i != 0) 
      {
        myprintf(" REALLOC");
        j = spec % i;
        myprintf(" %d ", j);
        myfflush(stdout);
        omTestRealloc(&cells[j], spec);
      }
    }
    else
    {
      myprintf(" ALLOC");
      myfflush(stdout);
      omTestAlloc(&cells[i], spec);
      i++;
      if (i % 1000 == 0)
      {
        printf("%d\n", i / 1000);
        fflush(stdout);
      }
    }
    myprintf("\n");
    myfflush(stdout);
    if (CHECK_LEVEL > 2)
    {
      for (j=0; j<i; j++)
      {
        omTestCheck(&cells[j]);
      }
    }
  }
  return 0;
}
      
        
