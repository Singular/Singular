LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
intvec i1 = 1,2,1; intvec i2 = 2,2,1; intvec i3 = 1,1; intvec i4 = 2,1,1,1;
// the corresponding monomials are xyx,y^2x,x^2,yx^3
list M = i1,i2,i3,i4;
M;
ivOrdMisLex(M);// orders the list of monomials
tst_status(1);$
