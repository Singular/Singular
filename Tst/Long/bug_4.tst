LIB "tst.lib";
tst_init();

LIB "finvar.lib";
ring R=7,(x,y),dp;
matrix A[2][2]=0,1,-1,0;
matrix B[2][2]=1,1,0,1;
list T=invariant_ring(A,B,intvec(1,0,1));

tst_status(1);$
