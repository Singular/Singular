LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
var(1);
var(5);
var(9);
x*x*z;
y*y*x*x;
z*y*x*z*z;
tst_status(1);$
