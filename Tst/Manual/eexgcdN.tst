LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,x,dp;
eexgcdN(list(24,15,21));
tst_status(1);$
