LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x4,x3,x2,x1),Dp;
int upToDeg = 7;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x4*x4-68*x4*x3-25*x4*x2-35*x4*x1+26*x3*x4+43*x2*x2-x1*x4-86*x1*x3-59*x1*x2+x1*x1,
x4*x4-4*x4*x3+13*x4*x2+12*x4*x1-9*x3*x4+18*x3*x3+4*x3*x2+41*x3*x1-143*x2*x2-7*x1*x4-20*x1*x3+15*x1*x2-x1*x1,
x4*x4+12*x4*x3+40*x4*x2+29*x4*x1-28*x3*x4-24*x3*x3-96*x3*x2+76*x3*x1-15*x2*x4+9*x1*x4+74*x1*x3+4*x1*x2+x1*x1,
x4*x4-34*x4*x3-17*x4*x2-37*x4*x1-96*x2*x3-5*x2*x2-42*x1*x4-34*x1*x2+x1*x1,
x4*x4-20*x4*x3+21*x4*x2-74*x4*x1-124*x2*x2-24*x2*x1-44*x1*x3+24*x1*x2-x1*x1,
x4*x4+74*x4*x2-150*x4*x1-9*x3*x4+40*x2*x2+15*x2*x1+48*x1*x4-96*x1*x3+x1*x2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
