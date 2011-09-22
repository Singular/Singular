LIB "tst.lib"; tst_init();
  ring R=32003,(x,y,z),ds;
  ideal i=x2+y12,y13;
  std(i);
  noether=x11;
  std(i);
  noether=0; //disables noether
tst_status(1);$
