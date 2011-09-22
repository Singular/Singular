LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=3,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=primary_invariants(A,intvec(1,1,0));
// In that example, there are no secondary invariants
// in degree 1 or 2.
matrix S,IS=secondary_and_irreducibles_no_molien(L[1..2],intvec(1,2),1);
print(S);
print(IS);
tst_status(1);$
