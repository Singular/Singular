LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),(dp(1),dp(2));
int degree_bound = 5;
def R = makeLetterplaceRing(5);
setring R;
ideal I = -x*y-7*y*y+3*x*x, x*y*x-y*x*y;
ideal J = letplaceGBasis(I);
J;
// now transfom letterplace polynomials into strings of words
lp2lstr(J,r); // export an object called @code{@LN} to the ring r
setring r;  // change to the ring r
lst2str(@LN,1);
tst_status(1);$
