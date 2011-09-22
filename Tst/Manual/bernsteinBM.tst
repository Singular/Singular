LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z,w),Dp;
poly F = x^3+y^3+z^2*w;
printlevel = 0;
bernsteinBM(F);
tst_status(1);$
