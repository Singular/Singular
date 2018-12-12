LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp(1),dp(2));
def A = makeLetterplaceRing(2);
setring A;  A;
isFreeAlgebra(A);
lpDegBound(A);  // degree bound
lpBlockSize(A); // number of variables in the main block
setring r; def B = makeLetterplaceRing(2,1); // to compare:
setring B;  B;
tst_status(1);$
