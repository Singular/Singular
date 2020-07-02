#ifndef SI_LOG2_H
#define SI_LOG2_H
#include "factory/factoryconf.h"
// stolen from:
// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
static inline int SI_LOG2(int v)
{
  const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  const unsigned int S[] = {1, 2, 4, 8, 16};

  unsigned int r = 0; // result of log2(v) will go here
  if (v & b[4]) { v >>= S[4]; r |= S[4]; }
  if (v & b[3]) { v >>= S[3]; r |= S[3]; }
  if (v & b[2]) { v >>= S[2]; r |= S[2]; }
  if (v & b[1]) { v >>= S[1]; r |= S[1]; }
  if (v & b[0]) { v >>= S[0]; r |= S[0]; }
  return (int)r;
}
#if SIZE_OF_LONG==4
#define SI_LOG2_LONG(A) SI_LOG2(A)
#else
static inline int SI_LOG2_LONG(long v)
{
  const unsigned long b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000, 0xFFFFFFFF00000000UL};
  const unsigned int S[] = {1, 2, 4, 8, 16, 32};

  unsigned int r = 0; // result of log2(v) will go here
  if (v & b[5]) { v >>= S[5]; r |= S[5]; }
  if (v & b[4]) { v >>= S[4]; r |= S[4]; }
  if (v & b[3]) { v >>= S[3]; r |= S[3]; }
  if (v & b[2]) { v >>= S[2]; r |= S[2]; }
  if (v & b[1]) { v >>= S[1]; r |= S[1]; }
  if (v & b[0]) { v >>= S[0]; r |= S[0]; }
  return (int)r;
}
#endif
#endif
