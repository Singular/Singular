LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = 0,(x,y,z),ds;
matrix A[2][3] = 1,2,x,y,z,7;
print(A);
flatten(A);
tst_status(1);$
