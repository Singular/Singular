#pragma once

struct FixStr : public GC {
  const char *str;
  Int len;
};

Word Hash(FixStr fs);
int Cmp(FixStr fs1, FixStr fs2);
