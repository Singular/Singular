LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = ZZ,(x,y,z),dp;
ring R = freeAlgebra(r, 10);
ideal Id = y*x - 3*x*y, z*x + y*y, z*y-y*z+z*z;
option(redTail);
option(redSB);
option(prot);
std(Id);
tst_status(1);$
