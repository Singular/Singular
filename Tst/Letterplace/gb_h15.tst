LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(u,v,U,V),Dp;
int upToDeg = 18;
def R = makeLetterplaceRing(upToDeg);
setring(R);
ideal Id = u*U-1,
U*u-1,
v*V-1,
V*v-1,
v*U*v*v*U*v-1,
U*v*U*v*U*v-1,
u*u*u*u*u*u*v*u*v*u*u*u*u*u*u*v*u*v-1;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
