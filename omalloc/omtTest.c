#include "omtTest.h"

#if CHECK_LEVEL > 0
#define OM_CHECK CHECK_LEVEL
#endif

#include "omalloc.h"

omMemCell_t cells[MAX_CELLS];
int errors = 0;
int missed_errors = 0;
int used_regions = 0;
int seed;


#if CHECK_LEVEL > 0
void omtTestDebug(omMemCell cell)
{
  size_t size = GET_SIZE(cell->spec);
  size_t is_size;
  
  if (om_ErrorStatus != omError_NoError) return;
  if (cell->bin != NULL)
  {
    if (IS_ALIGNED(cell->spec))
      omDebugAddrAlignedBin(cell->addr, cell->bin);
    else
      omDebugAddrBin(cell->addr, cell->bin);
  }
  else
  {
    if (IS_ALIGNED(cell->spec))
      omDebugAddrAlignedSize(cell->addr, size);
    else
      omDebugAddrSize(cell->addr, size);
  }
  if (om_ErrorStatus != omError_NoError) return;
  
  if (!OM_IS_ALIGNED(cell->addr))
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR,
                  "addr:%p is unaligned", cell->addr);
    return;
  }

  if (IS_ALIGNED(cell->spec) && !OM_IS_STRICT_ALIGNED(cell->addr))
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR,
                  "addr:%p is not strict unaligned", cell->addr);
    return;
  }
  
  is_size = omSizeOfAddr(cell->addr);
  if (!OM_IS_ALIGNED(is_size))
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR,
                  "is_size == %u is unaligned", is_size);
    return;
  }
  if (is_size < size)
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR,
                  "is_size==%u < size==%u", is_size, size);
    return;
  }

  if (is_size >> LOG_SIZEOF_LONG != omSizeWOfAddr(cell->addr))
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR,
                  "is_sizeW==%u < sizeW==%u", is_size >> LOG_SIZEOF_LONG, omSizeWOfAddr(cell->addr));
    return;
  }
    
  TestAddrContent(cell->addr, (IS_ZERO(cell->spec) ? 0 : cell->spec), is_size);
}

void TestAddrContentEqual(void* s1, void* s2, size_t size)
{
  int i;
  size_t sizeW = OM_ALIGN_SIZE(size) >> LOG_SIZEOF_LONG;
  
  for (i=0; i<sizeW; i++)
  {
    if (((unsigned long*)s1)[i] != ((unsigned long*)s2)[i])
    {
       omReportError(omError_Unknown, omError_NoError, OM_FLR,
                     "s1[%u]==%d  !=  s2[%u]==%d", i, ((unsigned long*)s1)[i], i, ((unsigned long*)s2)[i]);
       return;
    }
  }
}

void TestAddrContent(void* addr, unsigned long value, size_t size)
{
  size_t sizeW = OM_ALIGN_SIZE(size) >> LOG_SIZEOF_LONG;
  int i;
  
  if (!OM_IS_ALIGNED(addr))
  {
    omReportError(omError_Unknown, omError_NoError, OM_FLR, 
                  "addr %p unaligned", addr);
    return;
  }
  
  for (i=0; i<sizeW; i++)
  {
    if (((unsigned long*)addr)[i] != value)
    {
      omReportError(omError_Unknown, omError_NoError, OM_FLR, 
                    "word %d modified: is %u should be %u", i, ((unsigned long*)addr)[i], value);
      return;
    }
  }
}
#endif

void InitCellAddrContent(omMemCell cell)
{
  size_t sizeW = omSizeWOfAddr(cell->addr);
  omMemsetW(cell->addr, (IS_ZERO(cell->spec) ? 0 : cell->spec), sizeW);
}
  
void omCheckCells(int n, int level, omMemCell_t* cells)
{
#if END_CHECK_LEVEL > 0
  int l = om_Opts.MinCheck;
  int i;
  
  omTestMemory(level);
  om_Opts.MinCheck = 1;
  for (i=0; i<n; i++)
  {
    omtTestDebug(&cells[i]);
    if (om_ErrorStatus != omError_NoError)
    {
      errors++;
      om_ErrorStatus = omError_NoError;
    }
  }
  om_Opts.MinCheck = l;
#endif
}


int size_range = RANGE_MIN;
int size_range_number = RANGE_MAX / RANGE_MIN;

int MyRandSpec()
{
  int spec = 1 + (int) ( ((double) SPEC_MAX)* ((double) rand())/(RAND_MAX + 1.0));
  
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


void TestAlloc(omMemCell cell, int spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;
    
    omtTestAllocDebug(cell, spec);
  }
  else
    omtTestAlloc(cell, spec);
  if (om_ErrorStatus != omError_NoError)
  {
    errors++;
    om_ErrorStatus = omError_NoError;
  }
}

void TestRealloc(omMemCell cell, int spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;
    
    omtTestReallocDebug(cell, spec);
  }
  else
    omtTestRealloc(cell, spec);
  if (om_ErrorStatus != omError_NoError)
  {
    errors++;
    om_ErrorStatus = omError_NoError;
  }
}

void TestDup(omMemCell cell, int spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;
    
    omtTestDupDebug(cell, spec);
  }
  else
    omtTestDup(cell, spec);

  if (om_ErrorStatus != omError_NoError)
  {
    errors++;
    om_ErrorStatus = omError_NoError;
  }
}

void TestFree(omMemCell cell)
{
  if (cell->addr != NULL)
  {
    if (DO_FREE_CHECK(cell->spec))
    {
      omtTestFreeDebug(cell);
    }
    else
    {
      omtTestFree(cell);
    }
    if (om_ErrorStatus != omError_NoError)
    {
      errors++;
      om_ErrorStatus = omError_NoError;
    }
  }
}

void my_exit()
{
  printf("\notTest Summary: ");
  if (errors || missed_errors || used_regions)
  {   
    printf("***FAILED***errors:%d, missed_errors:%d, used_regions:%d, seed=%d\n", errors, missed_errors, used_regions, seed);
    if (errors) exit(errors);
    if (missed_errors) exit(missed_errors);
    if (used_regions) exit(used_regions);
  }
  else
  {
    printf("OK\n");
    exit(0);
  }
}


int main(int argc, char* argv[])
{
  int i=0, error_test = 1;
  int spec, j;
  int n = 1;
  int n_cells = MAX_CELLS;
  int decr = 2;
  om_Opts.MinCheck = CHECK_LEVEL;
  om_Opts.Keep = KEEP_ADDR;
  seed = time(NULL);
  
  omInitRet_2_Info(argv[0]);
  omInitGetBackTrace();
  omInitInfo();
  om_Opts.PagesPerRegion = PAGES_PER_REGION;
  
  if (argc > 1) sscanf(argv[1], "%d", &error_test);
  if (argc > 2) sscanf(argv[2], "%d", &seed);
  srand(seed);

  if (argc > 3) sscanf(argv[3], "%d", &n);
  if (argc > 4) sscanf(argv[4], "%d", &decr);
  
  if (decr < 2) decr = 2;
  printf("seed == %d\n", seed);
  fflush(stdout);
  while (1)
  {
    if (i == n_cells)
    {
      i = 0;
      printf("\nCells: %d\n", n_cells);
      fflush(stdout);
      omCheckCells(n_cells, END_CHECK_LEVEL, cells);
      omPrintStats(stdout);
      omPrintInfo(stdout);
      if (om_Info.CurrentRegionsAlloc > 0) omPrintBinStats(stdout);
      fflush(stdout);
#if CHECK_LEVEL > 0 && TRACK_LEVEL > 0
      if (error_test && errors == 0)
      {
        missed_errors = omtTestErrors();
        if (missed_errors < 0) 
        {
          my_exit();
        }
      }
#endif
      while (i< n_cells)
      {
        TestFree(&cells[i]);
        i++;
      }
      omFreeKeptAddr();
      omPrintStats(stdout);
      omPrintInfo(stdout);
      if (om_Info.CurrentRegionsAlloc > 0) 
      {
        omPrintBinStats(stdout);
        used_regions += om_Info.CurrentRegionsAlloc;
      }
      omPrintUsedAddrs(stdout);
      i=0;
      n--;
      if (n <= 0 || n_cells <= 100)
      {
        my_exit();
      }
      else
      {
        n_cells = n_cells / decr;
      }
    }
    spec = MyRandSpec();
    myprintf("%d:%d:%d:%d", i, spec, GET_SIZE(spec), GET_TRACK(spec));
    myfflush(stdout);
    if (DO_FREE(spec))
    {
      if (i != 0) 
      {
        myprintf(" FREE");
        j = spec % i;
        myprintf(" %d ", j);
        myfflush(stdout);
        TestFree(&cells[j]);
        TestAlloc(&cells[j], spec);
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
        TestRealloc(&cells[j], spec);
      }
    }
    else if (DO_DUP(spec))
    {
      if (i != 0)
      {
        myprintf(" DUP");
        j = spec % i;
        myprintf(" %d ", j);
        myfflush(stdout);
        TestDup(&cells[j], spec);
      }
    }
    else
    {
      myprintf(" ALLOC");
      myfflush(stdout);
      TestAlloc(&cells[i], spec);
      i++;
      if (i % 1000 == 0)
      {
        printf("%d:", i / 1000);
        fflush(stdout);
      }
    }
    myprintf("\n");
    myfflush(stdout);
#if 0
    if (CHECK_LEVEL > 2)
    {
      for (j=0; j<i; j++)
      {
        omtTestDebug(&cells[j]);
      }
    }
#endif
  }
  return 0;
}
      
        
