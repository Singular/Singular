LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
poly p = (x+2)*(x-1)*(x-5);
boundBuFou(p,-3,5);
boundBuFou(p,-2,5);
tst_status(1);$
