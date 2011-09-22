LIB "tst.lib"; tst_init();
  ring r=32003,(x,y),ds;
  poly f=(x3+y5)^2+x2y7;
  ideal i=std(jacob(f));
  mult(i);
  mult(std(f));
tst_status(1);$
