LIB "tst.lib"; tst_init();
  ring r;
  ideal i=x2+y3,y3+z4;
  std(i);
  ideal k=_;
  k*k+x;
  size(_[3]);
tst_status(1);$
