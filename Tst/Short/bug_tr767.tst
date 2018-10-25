LIB "tst.lib";
tst_init();

LIB"freegb.lib";
ring r = integer,(x,y,z),dp;
int uptodeg = 5; int lV = 3;
def R = makeLetterplaceRing(uptodeg);
setring R;
poly f = x*z*y - 2*z*y + 3*x;
shiftPoly(f,1);
shiftPoly(f,2);

tst_status(1);$
