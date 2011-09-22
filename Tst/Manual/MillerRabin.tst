LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number x=2;
x=x^787-1;
MillerRabin(x,3);
tst_status(1);$
