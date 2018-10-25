LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y,z),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
poly p = x*x*z; poly q = y*y*x*x;
poly w= z*y*x*z*z;
// p,q,w are some polynomials we want to transform into their
// intvec representation
lp2iv(p); lp2iv(q); lp2iv(w);
tst_status(1);$
