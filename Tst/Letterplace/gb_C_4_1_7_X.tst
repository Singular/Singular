LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x4,x3,x2,x1),Dp;
int upToDeg = 5;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x4*x4-25*x4*x2-x1*x4-6*x1*x3-9*x1*x2+x1*x1,
x4*x4-4*x4*x3+13*x4*x2+12*x4*x1-9*x3*x4+4*x3*x2+41*x3*x1-7*x1*x4-x1*x2,
x4*x4+12*x4*x3-4*x3*x3-9*x3*x2+9*x1*x4+x1*x1,
x4*x4-14*x4*x3-17*x4*x2-5*x2*x2-42*x1*x4,
x4*x4-2*x4*x3+2*x4*x2-7*x4*x1-x2*x2-13*x2*x1-4*x1*x3+2*x1*x2-x1*x1,
x4*x4+7*x4*x2-15*x4*x1-9*x3*x4+4*x2*x2+15*x2*x1+x1*x2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
