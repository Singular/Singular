LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
isOnCurve(32003,5,7,list(10,16,1));
tst_status(1);$
