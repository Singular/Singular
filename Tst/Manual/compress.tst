LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=0,(x,y,z),ds;
matrix A[3][4]=1,0,3,0,x,0,z,0,x2,0,z2,0;
print(A);
print(compress(A));
module m=module(A); show(m);
show(compress(m));
intmat B[3][4]=1,0,3,0,4,0,5,0,6,0,7,0;
compress(B);
intvec C=0,0,1,2,0,3;
compress(C);
tst_status(1);$
