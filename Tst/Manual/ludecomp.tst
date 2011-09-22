LIB "tst.lib"; tst_init();
  ring r=0,(x),dp;
  matrix A[3][4]=1,2,3,4,1,1,1,1,2,2,1,1;
  list plu = ludecomp(A);
  print(plu[3]);                   // the matrix U of the decomposition
  print(plu[1]*A-plu[2]*plu[3]);   // should be the zero matrix
tst_status(1);$
