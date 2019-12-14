LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(h,f,e),Dp;
int upToDeg = 5;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = f*e-e*f+h,
h*e-e*h-2*e,
h*f-f*h+2*f,
4*e*f+h*h-2*h;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
