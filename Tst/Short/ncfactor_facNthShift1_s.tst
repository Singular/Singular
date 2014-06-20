LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x1,x2,s1,s2),dp;
matrix C[4][4] = 1,1,1,1,
                 1,1,1,1,
                 1,1,1,1,
                 1,1,1,1;
matrix D[4][4] = 0,0,s1,0,
                 0,0,0,s2,
                 -s1,0,0,0,
                 0,-s2,0,0;
def r = nc_algebra(C,D);
setring(r);
poly h = (x1^2*s1 + s2)*(x2*s1+1);
list lsng = facShift(h);
print(lsng);
testNCfac(lsng, h);

tst_status();
tst_status(1); $
