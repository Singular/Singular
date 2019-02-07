#include <stdlib.h>
#include "../adlib/config.h"

#define MORECORE malloc_morecore
#define MORECORE_CONTIGUOUS 0
#define MORECORE_CANNOT_TRIM 1
#define MORECORE_FAILURE NULL

#define HAVE_MMAP 0

#define USE_DL_PREFIX

void *malloc_morecore(Int n) {
  if (n >= 0)
    return malloc(n);
  else
    return NULL;
}

#include "dlmalloc.c"

#define CORE_MALLOC dlmalloc
#define CORE_FREE dlfree
#define GC_GETENV_SKIP 1
#define GC_CLIBDECL
#define GC_FASTCALL

#include "tinygc.c"
