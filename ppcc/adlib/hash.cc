#include "lib.h"

// DJB2a hash

#define DJB2_INIT 5381
#define DJB2_ITER(h, x) ((h * 33) + x)

Word Hash(const char *addr, Int n) {
  Word hash = DJB2_INIT;
  while (n-- > 0) {
    Byte byte = (Byte)(*addr++);
    hash = DJB2_ITER(hash, byte);
  }
  return hash;
}

Word Hash(const char *str) {
  Word hash = DJB2_INIT;
  while (*str) {
    Byte byte = (Byte)(*str++);
    hash = DJB2_ITER(hash, byte);
  }
  return hash;
}

Word Hash(Str *str) {
  return Hash(str->c_str(), str->len());
}

Word Hash(Word w) {
  Word hash = DJB2_INIT;
  while (w) {
    Byte byte = (Byte) w;
    hash = DJB2_ITER(hash, byte);
    w >>= 8;
  }
  return hash;
}

Word Hash(Int w) {
  return Hash((Word) w);
}
