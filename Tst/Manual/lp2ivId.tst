LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y,z),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
poly p = x(1)*x(2)*z(3); poly q = y(1)*y(2)*x(3)*x(4);
poly w = z(1)*y(2)*x(3)*z(4);
// p,q,w are some polynomials we want to transform into their
// intvec representation
ideal G = p,q,w;
// define the ideal containing p,q and w
lp2ivId(G); // and return the list of intmats for this ideal
tst_status(1);$
