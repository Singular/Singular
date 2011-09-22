LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x,t),dp;
matrix A[3][3]=1,x2,x,x2,6,4,x,4,1;
print(A);
charpoly(A,"t");
tst_status(1);$
