LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";

ring R = 0,(x,y,z),dp;
matrix C[3][3] = 1,1,1,1,1,1,1,1,1;
matrix D[3][3];
D[1,2] = 1;
def r = nc_algebra(C,D);
setring r;
poly L = (x+y)*(x-y);
list lsng = facSubWeyl(L);
lsng;
testNCfac(lsng, L);

tst_status();
tst_status(1); $