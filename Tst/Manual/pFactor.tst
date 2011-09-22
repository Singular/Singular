LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
list L=primList(1000);
pFactor(1241143,13,L);
number h=10;
h=h^30+25;
pFactor(h,20,L);
tst_status(1);$
