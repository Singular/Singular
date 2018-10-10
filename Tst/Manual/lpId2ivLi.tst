LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal L = x*x,y*y,x*y*x;
lpId2ivLi(L); // returns the corresponding intvecs as a list
tst_status(1);$
