LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring R = 0,(x,y),dp;
def r = nc_algebra(1,1);
setring(r);
poly h = (x^3+x+1)*(x^4+y*x+2);
facFirstWeyl(h);
tst_status();
poly h2 = (x^2*y+y)*(y+x*y);
tst_status();
tst_status(1); $