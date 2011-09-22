LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x),dp;
matrix A[4][4]=6,0,0,0,0,0,6,0,0,6,0,0,0,0,0,6;
print(A);
diag_test(A);
tst_status(1);$
