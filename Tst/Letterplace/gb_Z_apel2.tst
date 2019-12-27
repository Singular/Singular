LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = ZZ,(z,y,x),Dp;
ring R = freeAlgebra(r, 7);
ideal Id = y*x - 3*x*y - z, z*x - x*z +y, z*y-y*z-x;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
