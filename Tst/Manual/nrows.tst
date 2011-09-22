LIB "tst.lib"; tst_init();
  ring R;
  matrix M[2][3];
  nrows(M);
  nrows(freemodule(4));
  module m=[0,0,1];
  nrows(m);
  nrows([0,x,0]);
tst_status(1);$
