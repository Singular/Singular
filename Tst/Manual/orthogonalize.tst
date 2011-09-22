LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x),lp;
matrix A[4][4]=5,6,12,4,7,3,2,6,12,1,1,2,6,4,2,10;
print(A);
print(orthogonalize(A));
tst_status(1);$
