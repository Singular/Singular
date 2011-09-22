LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,x,dp;
list L = bFactor((x-4/3)*(x+3)^2*(x-5)^4); L;
intRoots(L);
tst_status(1);$
