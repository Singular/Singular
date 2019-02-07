#include "lib.h"

int Cmp(Int a, Int b) {
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return 0;
}

int Cmp(Word a, Word b) {
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return 0;
}
