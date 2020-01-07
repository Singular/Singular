LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(f1,f2,f3,f4),Dp;
int upToDeg = 10;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = f1*f2-f2*f1,
f1*f3*f3-2*f3*f1*f3+f3*f3*f1,
f1*f4-f4*f1,
f2*f3-f3*f2,
f2*f4*f4-2*f4*f2*f4+f4*f4*f2,
f1*f1*f3-2*f1*f3*f1+f3*f1*f1,
f3*f4*f4-2*f4*f3*f4+f4*f4*f3,
f2*f2*f4-2*f2*f4*f2+f4*f2*f2,
f3*f3*f3*f4-3*f3*f3*f4*f3+3*f3*f4*f3*f3-f4*f3*f3*f3;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
