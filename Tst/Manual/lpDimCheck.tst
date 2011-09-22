LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal G = x(1)*x(2), y(1)*y(2),x(1)*y(2)*x(3);
// Groebner basis
ideal I = x(1)*x(2), y(1)*x(2)*y(3), x(1)*y(2)*x(3);
// Groebner basis
lpDimCheck(G); // invokes procedure, factor algebra is of finite K-dimension
lpDimCheck(I); // invokes procedure, factor algebra is of infinite Kdimension
tst_status(1);$
