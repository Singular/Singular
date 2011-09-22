LIB "tst.lib"; tst_init();
LIB "ncfactor.lib";
ring R = 0,(x,y),dp;
def r = nc_algebra(1,1);
setring(r);
poly h = (x^2*y^2+x)*(x+1);
facFirstWeyl(h);
tst_status(1);$
