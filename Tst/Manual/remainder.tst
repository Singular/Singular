LIB "tst.lib"; tst_init();
LIB "zeroset.lib";
ring R = (0,a), x, lp;
minpoly = a2+1;
poly f =  x4 - 1;
poly g = x3 - 1;
remainder(f, g);
tst_status(1);$
