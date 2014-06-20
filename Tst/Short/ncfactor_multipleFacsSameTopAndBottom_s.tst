LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = x2d3+xd4+6x2d2+12xd3+d4+11x2d+52xd2+13d3+6x2+88xd+58d2+45x+95d+35;
list lsng =facWeyl(L);
lsng;
testNCfac(lsng, L);

tst_status();
tst_status(1); $
