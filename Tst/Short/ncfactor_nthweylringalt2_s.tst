LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
LIB "ncfactor.lib";
ring R = 0,(x1,d1,x2,d2,x3,d3),dp;
matrix C[6][6] = 1,1,1,1,1,1,
                 1,1,1,1,1,1,
                 1,1,1,1,1,1,
                 1,1,1,1,1,1,
                 1,1,1,1,1,1,
                 1,1,1,1,1,1;
matrix D[6][6] =  0, 1, 0,0,0,0,
                 -1, 0, 0,0,0,0,
                  0, 0, 0,1,0,0,
                  0, 0,-1,0,0,0,
                  0,0, 0,0,0,1,
                  0, 0,0,0,-1,0;
def r = nc_algebra(C,D);
setring(r);
poly h =x1*x2^2*x3^3*d1*d2^2+x2*x3^3*d2;
list lsng = facWeyl(h);
print(lsng);
testNCfac(lsng, h);

tst_status();
tst_status(1); $