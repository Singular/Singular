LIB "tst.lib"; tst_init();
  int i = 42;
  int j = i + 3; j;
  i = i * 3 - j; i;
  int k;   // assigning the default value 0 to k
  k;
tst_status(1);$
