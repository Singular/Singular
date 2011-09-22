LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=32003,(x,y,z),lp;
matrix A[3][3]=1,2,3,x,y,z,7,8,9;
print(A);
print(permrow(A,2,1));
tst_status(1);$
