LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  matrix(x);
  matrix(x, 1, 2);
  matrix(intmat(intvec(1,2,3,4), 2, 2));
  matrix(_, 2, 3);
  matrix(_, 2, 1);
tst_status(1);$
