LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=32003,(x,y,z),lp;
matrix A[3][3]=1,x,3,4,y,6,7,z,9;
print(A);
print(permcol(A,2,3));
tst_status(1);$
