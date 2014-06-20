LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x,y),dp;
def r = nc_algebra(1,1);
setring(r);
poly h = (x^3+x+1)*(x^4+y*x+2);
list lsng = facWeyl(h);
lsng;
testNCfac(lsng,h);
tst_status();
poly h2 = (x^2*y+y)*(y+x*y);
lsng = facWeyl(h2);
lsng;
testNCfac(lsng,h2);
lsng;
tst_status();
tst_status(1); $
