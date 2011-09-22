LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=0,(x,y,z),ds;
matrix A[3][3]=1,2,3,x,y,z,x2,y2,z2;
matrix B[2][2]=1,0,2,0; matrix C[1][4]=4,5,x,y;
print(A);
print(B);
print(C);
print(concat(A,B,C));
tst_status(1);$
