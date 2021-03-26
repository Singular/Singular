LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r;
module m = [x],[y],[1,z];
is_zero(m,1);
qring q = std(ideal(x2+y3+z2));
ideal j = x2+y3+z2-37;
is_zero(j);
tst_status(1);$
