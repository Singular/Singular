#include "lib.h"

void *GC::operator new(size_t size) {
  return GC_MALLOC(size);
}

void *PtrFreeGC::operator new(size_t size) {
  void *p = GC_MALLOC_ATOMIC(size);
  memset(p, 0, size);
  return p;
}
