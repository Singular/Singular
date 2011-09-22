LIB "tst.lib"; tst_init();
  int i=1;
  int j;
  i++; i;  i--; i;
  // ++ and -- do not return a value as in C, cannot assign
  j = i++;
  // the value of j is unchanged
  j; i;
  i+2, 2-i, 5^2;
  5 div 2, 8%3;
  -5 div 2, -5 / 2, -5 mod 2, -5 % 2;
  1<2, 2<=2;
tst_status(1);$
