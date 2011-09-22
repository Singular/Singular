LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=2,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=primary_invariants(A);
matrix S=secondary_not_cohen_macaulay(L[1],A);
print(S);
tst_status(1);$
