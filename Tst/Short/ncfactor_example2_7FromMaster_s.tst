LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = (xdd + xd+1+ (xd+5)*x)*(((x*d)^2+1)*d + xd+3+ (xd+7)*x);
list lsng = facWeyl(L);
lsng;
testNCfac(lsng, L);

tst_status();
tst_status(1); $
