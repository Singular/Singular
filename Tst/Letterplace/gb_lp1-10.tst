LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),Dp;
int upToDeg = 10;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = z*z*z*z+y*x*y*x-x*y*y*x-3*z*y*x*z,
x*x*x+y*x*y-x*y*x,
z*y*x-x*y*z+z*x*z;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
