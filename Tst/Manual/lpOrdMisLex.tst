LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal M = x*y*x, y*y*x, x*x, y*x*x*x;
// some monomials
lpOrdMisLex(M); // orders the monomials lexicographically
tst_status(1);$
