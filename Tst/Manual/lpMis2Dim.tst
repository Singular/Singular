LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal L = x*y,y*x*y;
// ideal containing the mistletoes
lpMis2Dim(L); // returns the K-dimension of the factor algebra
tst_status(1);$
