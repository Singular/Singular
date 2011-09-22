LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),dp;
int uptodeg = 5; int lV = 3;
def R = makeLetterplaceRing(uptodeg);
setring R;
poly f = x(1)*z(2)*y(3) - 2*z(1)*y(2) + 3*x(1);
shiftPoly(f,1);
shiftPoly(f,2);
tst_status(1);$
