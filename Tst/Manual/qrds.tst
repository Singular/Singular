LIB "tst.lib"; tst_init();
  ring r=(complex,50),(dummy),dp;
  matrix A[3][3]=-10,37,-5,-14,51,-10,-29,99,-18;
  bigint b = bigint(10)^100; number t = 1/b;
  list L=qrds(A,t,t,t); L;
tst_status(1);$
