LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r = 0,(x),dp;
matrix A[4][4] = 1,4,4,7,2,5,5,4,4,1,1,3,0,2,2,7;
print(gauss_nf(A));
tst_status(1);$
