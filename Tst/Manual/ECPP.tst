LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number N=1267985441;
ECPP(N);
tst_status(1);$
