LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),Dp;
int upToDeg = 11;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = y*x*y-z*y*z,
x*y*x-z*x*y,
z*x*z-y*z*x,
x*x*x-2*y*y*y+3*z*z*z-4*x*y*z+5*x*z*z-6*x*y*y+7*x*x*z-8*x*x*y;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
