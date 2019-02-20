LIB "tst.lib"; tst_init();
  proc t1
  {
    int i=2;
    while (i>0)
    { i=i-1; }
  }
  TRACE=3;
  t1();
  TRACE=0;
tst_status(1);$
