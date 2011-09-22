LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=32003,(x,y,z),lp;
matrix A[3][3]=1,2,3,4,5,6,7,8,9;
print(A);
print(multcol(A,2,xy));
tst_status(1);$
