LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring R = 0,(z,x),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = (x^2*z^2+x)*x;
list lsng = facWeyl(L);
lsng;
testNCfac(lsng, L);

tst_status();
tst_status(1); $