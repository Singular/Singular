#include <time.h>
#include <string.h>

#define TRACK_LEVEL   1
#define CHECK_LEVEL   1
// #define MAX_CELLS  500000   
#define MAX_CELLS     100000
// #define MAX_CELLS     1000
#define KEEP_ADDR     100
#define END_CHECK_LEVEL 5

#ifdef OM_TEST_MALLOC
#define OM_EMULATE_OMALLOC
#endif


#include "omStructs.h"

struct omMemCell_s
{
  void* addr;
  omBin bin;
  int spec;
};

typedef struct omMemCell_s omMemCell_t;
typedef omMemCell_t* omMemCell;

extern omMemCell_t cells[];
void TestAlloc(omMemCell cell, int spec);
void TestRealloc(omMemCell cell, int spec);
void TestFree(omMemCell cell);

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
#define IS_ZERO(spec)       (spec & (1 << 16))
#define IS_BIN(spec)        (spec & (1 << 17))
#define IS_SPEC_BIN(spec)   (spec & (1 << 18))
#define IS_INLINE(spec)     (spec & (1 << 19))
#define DO_FREE(spec)       (!(spec & (1 << 20))  && !(spec & (1 << 21)))
#define DO_REALLOC(spec)    ((spec & (1 << 20))  && (spec & (1 << 21)))
#define DO_DUP(spec)        ((spec & (1 << 20)) && ! (spec & (1 << 21)))
#if CHECK_LEVEL > 0
//#define DO_CHECK(spec)      1
#define DO_CHECK(spec)      (spec & (1 << 22))
#define DO_FREE_CHECK(spec) (spec & (1 << 23))
#else
#define DO_CHECK(spec)      0
#define DO_FREE_CHECK(spec) 0
#endif
#if CHECK_LEVEL > 0 && TRACK_LEVEL > 0
#define DO_TRACK(spec)      (spec & (1 << 24))
#define GET_TRACK(spec)     (((spec & ((1 << 27) | (1 << 26) | (1 << 25))) >> 25) % 5) + TRACK_LEVEL
// #define DO_TRACK(spec)      TRACK_LEVEL
// #define GET_TRACK(spec)     TRACK_LEVEL
#else
#define DO_TRACK(spec)      0
#define GET_TRACK(spec)     0
#endif
#define IS_FREE_SIZE(spec)  (spec & (1 << 28))
#define IS_FREE_BIN(spec)   (spec & (1 << 29))
#define IS_SLOPPY(spec)     (spec & (1 << 30))


#define SPEC_MAX   (((unsigned long) (1 << 31)) -1)
#define SIZE_MAX  ((1 << 14) -1)
#define RANGE_MIN (1 << 6)
#define RANGE_MAX (1 << 14)

#define PAGES_PER_REGION 128

void omTestAlloc(omMemCell cell, int spec);
void omTestRealloc(omMemCell cell, int spec);
void omTestDup(omMemCell cell, int spec);
void omTestFree(omMemCell cell);

void omTestAllocDebug(omMemCell cell, int spec);
void omTestReallocDebug(omMemCell cell, int spec);
void omTestDupDebug(omMemCell cell, int spec);
void omTestFreeDebug(omMemCell cell);
void InitCellAddrContent(omMemCell cell);
int omTestErrors();

#if CHECK_LEVEL > 0
void omTestDebug(omMemCell cell);
void TestAddrContent(void* addr, unsigned long value, size_t size);
void TestAddrContentEqual(void* s1, void* s2, size_t size);
#else
#define omTestDebug(cell)               ((void)0)
#define TestAddrContent(a,v,s)          ((void)0)
#define TestAddrContentEqual(s1, s2, s) ((void)0)
#endif
