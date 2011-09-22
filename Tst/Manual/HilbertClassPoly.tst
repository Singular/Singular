LIB "tst.lib"; tst_init();
LIB "atkins.lib";
ring r = 0,x,dp;
number D=-23;
HilbertClassPoly(D,50);
tst_status(1);$
