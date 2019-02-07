#include <stdio.h>
#include <math.h>

#define IND "  "

const char *regs[] = {"abcd", "dabc", "cdab", "bcda"};
const int slist[4][4] = {
  { 7, 12, 17, 22 },
  { 5, 9, 14, 20 },
  { 4, 11, 16, 23 },
  { 6, 10, 15, 21 }
};

const char *flist[] = { "F", "G", "H", "I" };

int klist[4][2] = { { 0, 1 }, { 1, 5 }, { 5, 3 }, { 0, 7 } };

unsigned long tvals[64];

void gen(int n) {
  int t = n * 16;
  int k = klist[n][0];
  int step = klist[n][1];
  const char * f = flist[n];
  printf(IND "// Round %d\n", n+1);
  printf(IND "#define U(a, b, c, d, k, s, t) \\\n"
    IND "  a = b + ROT(a + %s(b,c,d) + buf[k] + t, s)\n",
    flist[n]);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      char a = regs[j][0];
      char b = regs[j][1];
      char c = regs[j][2];
      char d = regs[j][3];
      int s = slist[n][j];
      unsigned long t = tvals[n*16 + i * 4 + j];
      printf(IND "U(%c, %c, %c, %c, %d, %d, 0x%lx);\n",
        a, b, c, d, k, s, t);
      k = (k + step) & 15;
      t++;
    }
  }
  printf(IND "#undef U\n");
}

int main() {
  for (int i = 0; i < 64; i++) {
    tvals[i] = (unsigned long) floor((1UL << 32) * fabs(sin((double) (i+1))));
  }
  printf(
    IND "#define F(x, y, z) ((x & y) | ((~(x) & z)))\n"
    IND "#define G(x, y, z) ((x & z) | ((y & ~(z))))\n"
    IND "#define H(x, y, z) ((x) ^ (y) ^(z))\n"
    IND "#define I(x, y, z) ((y) ^ ((x) | ~(z)))\n"
    IND "#define ROT(x, s) ((x) << (s) | (x) >> (32-s))\n"
  );
  for (int i = 0; i < 4; i++)
    gen(i);
  printf(
    IND "#undef F\n"
    IND "#undef G\n"
    IND "#undef H\n"
    IND "#undef I\n"
    IND "#undef ROT\n"
  );
  return 0;
}
