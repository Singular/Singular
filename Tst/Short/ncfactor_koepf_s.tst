LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = (x^4-1)*x*d^2+(1+7*x^4)*d+8*x^3;
def l=facFirstWeyl(L);
testNCfac (l, L);
l;

tst_status();
tst_status(1); $
