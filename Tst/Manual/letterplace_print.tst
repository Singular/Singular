LIB "tst.lib"; tst_init();
LIB "freegb.lib";

ring r = 0,(x,y,z),dp;
ring R = freeAlgebra(r, 5);
var(1);
var(5);
var(9);
x*x*z;
y*y*x*x;
z*y*x*z*z;

tst_status(1);$
