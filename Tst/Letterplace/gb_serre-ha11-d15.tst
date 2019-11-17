LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(f1,f2,f3),Dp;
int upToDeg = 15;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = f1*f2*f2-2*f2*f1*f2+f2*f2*f1,
f1*f3-f3*f1,
f1*f1*f2-2*f1*f2*f1+f2*f1*f1,
f2*f3*f3*f3-3*f3*f2*f3*f3+3*f3*f3*f2*f3-f3*f3*f3*f2,
f2*f2*f2*f3-3*f2*f2*f3*f2+3*f2*f3*f2*f2-f3*f2*f2*f2;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
