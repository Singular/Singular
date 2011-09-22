LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring R=32003,(x,y,z),lp;
matrix A[4][4]=x,y,z,0,1,2,3,4,5,6,7,8,9,x2,y2,z2;
print(A);
intvec v=1,3,4;
matrix B=submat(A,v,1..3);
print(B);
tst_status(1);$
