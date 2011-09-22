LIB "tst.lib"; tst_init();
LIB "zeroset.lib";
ring R = (0,a), x, lp;
minpoly = a2+1;
poly f =  x4 - 2*x + 1;
sqfrNorm(f);
tst_status(1);$
