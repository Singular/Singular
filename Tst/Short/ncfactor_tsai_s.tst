LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = (x^6+2*x^4-3*x^2)*d^2-(4*x^5-4*x^4-12*x^2-12*x)*d + (6*x^4-12*x^3-6*x^2-24*x-12);
def l=facFirstWeyl(L);
testNCfac (l, L);
l;

tst_status();
tst_status(1); $
