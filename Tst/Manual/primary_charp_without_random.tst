LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=2,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
matrix P=primary_charp_without_random(A,1);
print(P);
tst_status(1);$
