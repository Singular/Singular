LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring R = 0,(n,s),dp;
def r = nc_algebra(1,s);
setring(r);
poly h =n^2+3*n+2+(-2*n-4)*s+s^2;
list lsng = facShift(h);
lsng;
testNCfac(lsng, h);

tst_status();
tst_status(1); $