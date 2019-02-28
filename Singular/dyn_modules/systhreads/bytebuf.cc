#include "bytebuf.h"

char *allocate_space(size_t n) {
  return new char[n];
}

void free_space(size_t n, char *p) {
  delete [] p;
}
