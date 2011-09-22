LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),dp;
def R=nc_algebra(-1,0); // an anticommutative algebra
setring R;
ideal m = maxideal(1);
m;
poly f = x2;
poly g = y3;
ideal i = x*y*z , f-g, g*(x-y) + f^4 ,0, 2x-z2y;
ideal M = i + maxideal(10);
i = M*M;
ncols(i);
i = M^2;
ncols(i);
i[ncols(i)];
vector v = [x,y-z,x2,y-x,x2yz2-y];
ideal j = ideal(v);
j;
tst_status(1);$
