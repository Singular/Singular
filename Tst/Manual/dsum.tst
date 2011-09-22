LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r = 0,(x,y,z),ds;
matrix A[3][3] = 1,2,3,4,5,6,7,8,9;
matrix B[2][2] = 1,x,y,z;
print(A);
print(B);
print(dsum(A,B));
tst_status(1);$
