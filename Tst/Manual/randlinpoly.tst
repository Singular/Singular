LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z,w),dp;
poly p = randlinpoly();
p;
randlinpoly(5);
tst_status(1);$
