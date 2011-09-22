LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(a,b,c,d),dp;
def GEN=list(list(list(1,3),list(2,4)));
matrix G = invariant_algebra_perm(GEN,1);
G;
tst_status(1);$
