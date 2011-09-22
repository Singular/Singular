LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal M = x(1)*y(2)*x(3), y(1)*y(2)*x(3), x(1)*x(2), y(1)*x(2)*x(3)*x(4);
// some monomials
lpOrdMisLex(M); // orders the monomials lexicographically
tst_status(1);$
