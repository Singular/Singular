LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
list L=ellipticAllPoints(181,71,150);
size(L);
L[size(L)];
tst_status(1);$
