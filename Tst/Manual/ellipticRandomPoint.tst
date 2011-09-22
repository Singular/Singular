LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
ellipticRandomPoint(32003,3,181);
tst_status(1);$
