LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring r = 0,(x,y,z),dp;
matrix D[3][3]; D[1,3]=-1;
def R = nc_algebra(1,D); // x,z generate Weyl subalgebra
setring R;
poly h = (x^2*z^2+x)*x;
list lsng = facSubWeyl(h,x,z);
lsng;
testNCfac(lsng, h);

tst_status();
tst_status(1); $