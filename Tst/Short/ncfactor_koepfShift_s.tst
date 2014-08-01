LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(n,s),dp;
def r = nc_algebra(1,s);
setring(r);
poly h = n*(n+1)*s^2-2*n*(n+100)*s+(n+99)*(n+100);
list lsng = facShift(h);
lsng;
testNCfac(lsng, h);

tst_status();
tst_status(1); $
