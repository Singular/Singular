LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x4,x3,x2,x1),Dp;
int upToDeg = 7;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x4*x4-5*x4*x2+6*x1*x4-9*x1*x2+x1*x1,
x4*x4-4*x4*x3+13*x4*x2-9*x3*x4+11*x3*x1-7*x1*x4,
x3*x3-7*x3*x2+9*x1*x4+x1*x1,
11*x4*x3+7*x4*x2-5*x2*x2-4*x1*x4,
x4*x2-7*x4*x1-x2*x2-13*x2*x1-4*x1*x3+2*x1*x2-x1*x1,
15*x4*x1-9*x3*x4+4*x2*x2+15*x2*x1+x1*x2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
