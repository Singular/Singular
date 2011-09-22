LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
R;
setring R; // sets basering to Letterplace ring
intvec i1 = 1,2; intvec i2 = 2,1,2;
// the mistletoes are xy and yxy, which are already ordered lexicographically
list L = i1,i2;
ivMis2Base(L); // returns the basis of the factor algebra
tst_status(1);$
