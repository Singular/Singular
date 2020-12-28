LIB "tst.lib"; tst_init();
LIB"freegb.lib";

ring r = 0,(x,y,z,w),dp;
ring r2 = 0,(a,b,c),dp;

ring R = freeAlgebra(r, 5);
ring R2 = freeAlgebra(r2, 10);

setring R;
poly j1 = y*x*z + x*z;
poly j2 = z * j1;
poly j3 = j1 * z;
ideal J = j1, j2, j3;

setring R2;
map f = R,a,b,c;
map g = R,c*a*b*a+b+c,b,c;

f(J);
f(j1);
f(j2);
f(j3);

g(J);
tst_status(1);$