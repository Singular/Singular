LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = ZZ,(z,y,x),Dp;
ring R = freeAlgebra(r, 7);
ideal Id = y*x - 3*x*y - 3*z, z*x - 2*x*z +y, z*y-y*z-x;
option(redTail);
option(redSB);
std(Id);
tst_status(1);$
