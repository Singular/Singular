LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),Dp;
int upToDeg = 11;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = y*x*y-z*y*z,
x*y*x-z*x*y,
z*x*z-y*z*x,
x*x*x+y*y*y+z*z*z+x*y*z;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
