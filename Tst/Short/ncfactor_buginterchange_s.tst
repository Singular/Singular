LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = x^3*(x*d^2 - x*d + 4*d-2);
list lsng = facWeyl(L);
lsng;
testNCfac(lsng, L);

tst_status();
tst_status(1); $