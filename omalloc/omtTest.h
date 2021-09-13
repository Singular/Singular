#include <time.h>
#include <string.h>
#include <limits.h>

#include "omConfig.h"

#ifdef HAVE_OMALLOC

#define TRACK_LEVEL   1
#define CHECK_LEVEL   1
// keep every ith address: define to 0 if no keeping
#define KEEP_LEVEL    20
// #define KEEP_LEVEL    0

// #define MAX_CELLS  500000
// #define MAX_CELLS     100000
#define MAX_CELLS     100000
#define KEEP_ADDR     100
// #define KEEP_ADDR     0
#define END_CHECK_LEVEL 5

#ifdef OM_TEST_MALLOC
#define OM_EMULATE_OMALLOC
#endif


#include "omStructs.h"

struct omMemCell_s
{
  void* addr;
  omBin bin;
  unsigned long spec;
  omBin orig_bin;
};

typedef struct omMemCell_s omMemCell_t;
typedef omMemCell_t* omMemCell;

extern omMemCell_t cells[];
void TestAlloc(omMemCell cell, unsigned long spec);
void TestRealloc(omMemCell cell, unsigned long spec);
void TestFree(omMemCell cell);

#if CHECK_LEVEL > 2
#define myprintf(format, args...) \
  printf(format, ## args)
#define myfflush(what) fflush(what)
#else
#define myprintf(format, args...) do {} while (0)
#define myfflush(what)            do {} while (0)
#endif

#define IS_STICKY_BIN(spec) (spec & 1)
// #define IS_STICKY_BIN(spec) (0)
#define GET_SIZE(spec)      (spec & ((((unsigned long) 1) << 14) -1))
#define SET_SIZE(spec, size) spec = ((spec & ~((((unsigned long) 1) << 14) -1)) | (size))
#define IS_ALIGNED(spec)    (spec & (((unsigned long) 1) << 15))
#define IS_ZERO(spec)       (spec & (((unsigned long) 1) << 16))
#define IS_BIN(spec)        (spec & (((unsigned long) 1) << 17))
#define IS_SPEC_BIN(spec)   (spec & (((unsigned long) 1) << 18))
#define IS_INLINE(spec)     (spec & (((unsigned long) 1) << 19))
#define DO_FREE(spec)       (!(spec & (((unsigned long) 1) << 20))  && !(spec & (((unsigned long) 1) << 21)))
#define DO_REALLOC(spec)    ((spec & (((unsigned long) 1) << 20))  && (spec & (((unsigned long) 1) << 21)))
#define DO_DUP(spec)        ((spec & (((unsigned long) 1) << 20)) && ! (spec & (((unsigned long) 1) << 21)))
#if CHECK_LEVEL > 0
//#define DO_CHECK(spec)      1
#define DO_CHECK(spec)      (spec & (((unsigned long) 1) << 22))
#define DO_FREE_CHECK(spec) (spec & (((unsigned long) 1) << 23))
#else
#define DO_CHECK(spec)      0
#define DO_FREE_CHECK(spec) 0
#endif
#if CHECK_LEVEL > 0 && TRACK_LEVEL > 0
#define DO_TRACK(spec)      (spec & (((unsigned long) 1) << 24))
#define GET_TRACK(spec)     (((spec & ((((unsigned long) 1) << 27) | (((unsigned long) 1) << 26) | (((unsigned long) 1) << 25))) >> 25) % 5) + TRACK_LEVEL
// #define DO_TRACK(spec)      TRACK_LEVEL
// #define GET_TRACK(spec)     TRACK_LEVEL
#else
#define DO_TRACK(spec)      0
#define GET_TRACK(spec)     0
#endif
#if KEEP_LEVEL > 0
#define DO_KEEP(spec)           (DO_CHECK(spec) && (spec % KEEP_LEVEL == 0))
#define DO_FREE_KEEP(spec)      (DO_FREE_CHECK(spec) && (spec % KEEP_LEVEL == 0))
#else
#define DO_KEEP(spec)       0
#define DO_FREE_KEEP(spec)  0
#endif

#define IS_FREE_SIZE(spec)      (spec & (((unsigned long) 1) << 28))
#define IS_FREE_BIN(spec)       (spec & (((unsigned long) 1) << 29))
#define IS_SLOPPY(spec)         (spec & (((unsigned long) 1) << 30))
#define IS_FREE_BINADDR(spec)   (spec & (((unsigned long) 1) << 31))


#define SPEC_MAX   ULONG_MAX
#define SIZE_MAX  ((((unsigned long) 1) << 14) -1)
#define RANGE_MIN (((unsigned long) 1) << 6)
#define RANGE_MAX (((unsigned long) 1) << 14)

#define PAGES_PER_REGION 128

void omtTestAlloc(omMemCell cell, unsigned long spec);
void omtTestRealloc(omMemCell cell, unsigned long spec);
void omtTestDup(omMemCell cell, unsigned long spec);
void omtTestFree(omMemCell cell);

void omtTestAllocDebug(omMemCell cell, unsigned long spec);
void omtTestReallocDebug(omMemCell cell, unsigned long spec);
void omtTestDupDebug(omMemCell cell, unsigned long spec);
void omtTestFreeDebug(omMemCell cell);

void omtTestAllocKeep(omMemCell cell, unsigned long spec);
void omtTestReallocKeep(omMemCell cell, unsigned long spec);
void omtTestDupKeep(omMemCell cell, unsigned long spec);
void omtTestFreeKeep(omMemCell cell);

void InitCellAddrContent(omMemCell cell);
int omtTestErrors();
omBin omtGetStickyBin(omBin bin);

#if CHECK_LEVEL > 0
void omtTestDebug(omMemCell cell);
void TestAddrContent(void* addr, unsigned long value, size_t size);
void TestAddrContentEqual(void* s1, void* s2, size_t size);
#else
#define omtTestDebug(cell)               do {} while (0)
#define TestAddrContent(a,v,s)          do {} while (0)
#define TestAddrContentEqual(s1, s2, s) do {} while (0)
#endif
#endif
