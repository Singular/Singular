LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,x,dp;
exgcdN(24,15);
tst_status(1);$
