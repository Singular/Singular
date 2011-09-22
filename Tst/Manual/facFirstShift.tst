LIB "tst.lib"; tst_init();
LIB "ncfactor.lib";
ring R = 0,(x,s),dp;
def r = nc_algebra(1,s);
setring(r);
poly h = (s^2*x+x)*s;
facFirstShift(h);
tst_status(1);$
