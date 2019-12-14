LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x4,x3,x2,x1),Dp;
int upToDeg = 7;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x4*x4-25*x4*x2-x1*x4-6*x1*x3-9*x1*x2+x1*x1,
x4*x3+13*x4*x2+12*x4*x1-9*x3*x4+4*x3*x2+41*x3*x1-7*x1*x4-x1*x2,
x3*x3-9*x3*x2+2*x1*x4+x1*x1,
17*x4*x2-5*x2*x2-41*x1*x4,
x2*x2-13*x2*x1-4*x1*x3+2*x1*x2-x1*x1,
x2*x1+4*x1*x2-3*x1*x1;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
