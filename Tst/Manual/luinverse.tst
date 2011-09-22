LIB "tst.lib"; tst_init();
  ring r=0,(x),dp;
  matrix A[3][3]=1,2,3,1,1,1,2,2,1;
  list L = luinverse(A);
  if (L[1] == 1)
  {
    print(L[2]);
    "----- next should be the (3 x 3)-unit matrix:";
    print(A*L[2]);
  }
tst_status(1);$
