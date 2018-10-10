LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal G = x*x, y*y,x*y*x; // ideal G contains a
//Groebner basis
lpHilbert(G,5,2); // invokes procedure with degree bound 5 and 2 variables
// note that the optional parameters are not necessary, due to the finiteness
// of the K-dimension of the factor algebra
lpDHilbert(G); // procedure with ring parameters
lpDHilbert(G,0); // procedure without degreebound
tst_status(1);$
