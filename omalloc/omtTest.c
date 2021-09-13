#include "omtTest.h"

#if CHECK_LEVEL > 0
#define OM_CHECK CHECK_LEVEL
#endif

#include "omalloc.h"

#ifdef HAVE_OMALLOC

omMemCell_t cells[MAX_CELLS];
int errors = 0;
int missed_errors = 0;
int used_regions = 0;
int seed;

#if defined (__hpux) || defined (__alpha)  || defined (__svr4__) || defined (__SVR4)
/* SF1 cosimo.medicis.polytechnique.fr V4.0 1229 alpha works */
#if defined (__hpux) || defined (__svr4__) || defined (__SVR4)
/* HPUX lacks random().  DEC OSF/1 1.2 random() returns a double.  */
long mrand48 ();
void srand48();
static long
random ()
{
  return mrand48 ();
}
static void srandom(long seed)
{
  srand48(seed);
}
#endif
#endif

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
    if ((i % 10000) == 0)
    {
      printf(".");
      fflush(stdout);
    }
  }
  om_Opts.MinCheck = l;
#endif
}


int size_range = RANGE_MIN;
int size_range_number = RANGE_MAX / RANGE_MIN;

int MyRandSpec()
{
  unsigned long spec = random() + 1;
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


void TestAlloc(omMemCell cell, unsigned long spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;

    if (DO_KEEP(spec))
      omtTestAllocKeep(cell, spec);
    else
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

void TestRealloc(omMemCell cell, unsigned long spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;

    if (DO_KEEP(spec))
      omtTestReallocKeep(cell, spec);
    else
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

void TestDup(omMemCell cell, unsigned long spec)
{
  if (DO_CHECK(spec))
  {
    if (DO_TRACK(spec))
      om_Opts.MinTrack = GET_TRACK(spec);
    else
      om_Opts.MinTrack = 0;

    if (DO_KEEP(spec))
      omtTestDupKeep(cell, spec);
    else
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
      if (DO_KEEP(cell->spec))
        omtTestFreeKeep(cell);
      else
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

omBin omtGetStickyBin(omBin bin)
{
  omBin sticky_bin = omFindInGList(om_StickyBins, next, max_blocks, bin->max_blocks);
  if (sticky_bin == NULL)
    sticky_bin = omGetStickyBinOfBin(bin);
  return sticky_bin;
}

void omtMergeStickyBins(omMemCell cell, int n)
{
  int i;
  omBin bin;

  for (i=0; i<n; i++)
  {
    if (cell[i].orig_bin != NULL)
    {
      if (omIsOnGList(om_StickyBins, next, cell[i].bin))
        omMergeStickyBinIntoBin(cell[i].bin, cell[i].orig_bin);

      cell[i].bin = cell[i].orig_bin;
      cell[i].orig_bin = NULL;
    }
  }

  bin = om_StickyBins;
  while (bin != NULL)
  {
    if (bin->current_page == om_ZeroPage)
    {
      omBin next_bin = bin->next;
      om_StickyBins = omRemoveFromGList(om_StickyBins, next, bin);
      __omFreeBinAddr(bin);
      bin = next_bin;
    }
    else
    {
      bin = bin->next;
    }
  }
}


void my_exit()
{
  printf("\nomtTest Summary: ");
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
  unsigned long spec, j;
  int n = 1;
  int n_cells = MAX_CELLS;
  int decr = 2;
  int last_kept_freed = 0;
  om_Opts.MinCheck = CHECK_LEVEL;
  om_Opts.Keep = KEEP_ADDR;

  seed = time(NULL);

  omInitRet_2_Info(argv[0]);
  omInitGetBackTrace();
  omInitInfo();
  om_Opts.PagesPerRegion = PAGES_PER_REGION;

  if (argc > 1) sscanf(argv[1], "%d", &error_test);
  if (argc > 2) sscanf(argv[2], "%d", &seed);
  srandom(seed);

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
      printf("\nCells: %d KeptAddr:%d AlwaysKeptAddr:%d\n", n_cells,
#ifndef OM_NDEBUG
             omListLength(om_KeptAddr), omListLength(om_AlwaysKeptAddrs)
#else
             0, 0
#endif
             );

      printf("Checking Memory and all cells ");
      fflush(stdout);
      omCheckCells(n_cells, END_CHECK_LEVEL, cells);
      printf("\n");
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
      omtMergeStickyBins(cells, n_cells);
      while (i< n_cells)
      {
        TestFree(&cells[i]);
        i++;
      }
      omFreeKeptAddr();
      omtMergeStickyBins(cells, -1);
      omPrintStats(stdout);
      omPrintInfo(stdout);
      if (om_Info.CurrentRegionsAlloc > 0)
      {
        omPrintBinStats(stdout);
        used_regions += om_Info.CurrentRegionsAlloc;
      }
      omPrintUsedAddrs(stdout, 5);
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
    myprintf("%d:%lu:%ld:%ld", i, spec, GET_SIZE(spec), GET_TRACK(spec));
    myfflush(stdout);
    if (DO_FREE(spec))
    {
      if (i != 0)
      {
        myprintf(" FREE");
        j = spec % i;
        myprintf(" %ld ", j);
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
        myprintf(" %ld ", j);
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
        myprintf(" %ld ", j);
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
    // free kept addresses from time to time
    if ((i % 10000) == 0 && i != n_cells && i!=last_kept_freed)
    {
      printf("F:");
      omFreeKeptAddr();
      last_kept_freed = i;
    }
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
#else

int main(int argc, char* argv[])
{
  return 0;
}
#endif
