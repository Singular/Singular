LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
list L=primList(1000);
factorLenstraECM(181*32003,L,10,5);
number h=10;
h=h^30+25;
factorLenstraECM(h,L,4,3);
tst_status(1);$
