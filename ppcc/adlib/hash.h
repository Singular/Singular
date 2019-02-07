#pragma once

Word Hash(const char *addr, Int n);
Word Hash(Str *str);
Word Hash(const char *str);
Word Hash(Word w);
Word Hash(Int w);

#define FIB_HASH_MULT_32 ((Word32) 0x9e3779b9)

static inline Word32 FibHash(Word32 w) {
  return w * FIB_HASH_MULT_32;
}

static inline Word32 FibHash(Word32 w, int bits) {
  return (w * FIB_HASH_MULT_32) >> (32 - bits);
}

#ifdef HAVE_64BIT_SUPPORT
#define FIB_HASH_MULT_64 ((Word64) 0x9e3779b97f4a7c13)

static inline Word64 FibHash(Word64 w) {
  return w * FIB_HASH_MULT_64;
}

static inline Word64 FibHash(Word64 w, int bits) {
  return (w * FIB_HASH_MULT_64) >> (64 - bits);
}

#endif
