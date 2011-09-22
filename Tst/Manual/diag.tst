LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = 0,(x,y,z),ds;
print(diag(xy,4));
matrix A[3][2] = 1,2,3,4,5,6;
print(A);
print(diag(A));
tst_status(1);$
