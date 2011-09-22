LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x),lp;
matrix A[2][2]=1,4,4,15;
print(A);
print(sym_gauss(A));
tst_status(1);$
