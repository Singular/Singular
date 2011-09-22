LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=primary_invariants(A);
matrix S,IS=secondary_char0(L[1..3],1);
print(S);
print(IS);
tst_status(1);$
