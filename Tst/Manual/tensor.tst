LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=32003,(x,y,z),(c,ds);
matrix A[3][3]=1,2,3,4,5,6,7,8,9;
matrix B[2][2]=x,y,0,z;
print(A);
print(B);
print(tensor(A,B));
tst_status(1);$
