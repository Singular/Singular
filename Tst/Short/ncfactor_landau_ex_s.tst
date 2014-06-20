LIB "tst.lib";
tst_init();
LIB "ncfactor.lib";

ring R = 0,(x1,x2,d1,d2),dp;
 matrix C[4][4] = 1,1,1,1,
                  1,1,1,1,
                  1,1,1,1,
                  1,1,1,1;
 matrix D[4][4] = 0,0,1,0,
                  0,0,0,1,
                  -1,0,0,0,
                  0,-1,0,0;
 def r = nc_algebra(C,D);
 setring(r);
poly h = (d1+1)^2*(d1 + x1*d2);
list lsng = facWeyl(h);
print(lsng);
testNCfac(lsng, h);

tst_status();
tst_status(1); $
