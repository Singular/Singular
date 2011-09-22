LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,x,dp;
gcdN(24,15);
tst_status(1);$
