LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(a,b,c,d),dp;
matrix A[4][4]=
0,0,1,0,
0,0,0,1,
1,0,0,0,
0,1,0,0;
list L = group_reynolds(A);
matrix G = invariant_algebra_reynolds(L[1],1);
G;
tst_status(1);$
