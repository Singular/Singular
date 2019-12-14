LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),Dp;
int upToDeg = 10;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = x*y+y*z,
x*x+x*y-y*x-y*y;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
