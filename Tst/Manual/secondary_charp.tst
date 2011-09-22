LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=3,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=primary_invariants(A);
matrix S,IS=secondary_charp(L[1..size(L)],1);
print(S);
print(IS);
tst_status(1);$
