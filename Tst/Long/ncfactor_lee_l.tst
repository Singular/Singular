LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,d),dp;
def r = nc_algebra(1,1);
setring(r);
poly L = 10x5d4+26x4d5+47x5d2-97x4d3;
def l=facFirstWeyl(L);
testNCfac (l, L);
l;

tst_status(1); $
