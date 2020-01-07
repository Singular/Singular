LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = 0,(x,y,z),dp;
ring R = freeAlgebra(r, 5);
ideal I = y*x*y - z*y*z, x*y*x - z*x*y, z*x*z - y*z*x, x*x*x + y*y*y + z*z*z + x*y*z;
ideal J = twostd(I);
poly p = y*x*y*z*y - y*z*z*y + z*y*z;
poly q = z*x*z*y*z - y*z*x*y*z;
reduce(p,J);
reduce(q,J);

tst_status(1);$
