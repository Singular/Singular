LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
ellipticRandomCurve(32003);
tst_status(1);$
