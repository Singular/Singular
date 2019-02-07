#include "adlib/lib.h"
#include "fixstr.h"

Word Hash(FixStr fs) {
  return Hash(fs.str, fs.len);
}

int Cmp(FixStr fs1, FixStr fs2) {
  Int len1 = fs1.len;
  Int len2 = fs2.len;
  int result = memcmp(fs1.str, fs2.str, Min(len1, len2));
  if (result == 0) {
    if (len1 < len2)
      result = -1;
    else if (len1 > len2)
      result = 1;
  }
  return result;
}
