LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x),dp;
matrix A[10][10]=random(2,10,10)+unitmat(10)*x;
print(A);
det_B(A);
tst_status(1);$
