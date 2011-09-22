LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
list L=primList(5000);
quadraticSieve(7429,3,L,4);
quadraticSieve(1241143,100,L,50);
tst_status(1);$
