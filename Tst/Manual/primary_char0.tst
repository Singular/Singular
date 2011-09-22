LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
matrix REY,M=reynolds_molien(A);
matrix P=primary_char0(REY,M);
print(P);
tst_status(1);$
