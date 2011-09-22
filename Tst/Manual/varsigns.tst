LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
list l = 1,2,3;
varsigns(l);
l = 1,-1,2,-2,3,-3;
varsigns(l);
tst_status(1);$
