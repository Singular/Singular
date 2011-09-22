LIB "tst.lib"; tst_init();
  LIB "mondromy.lib";
  ring R=0,(x,y),ds;
  poly f=x5+x2y2+y5;
  matrix M=monodromyB(f);
  print(M);
tst_status(1);$
