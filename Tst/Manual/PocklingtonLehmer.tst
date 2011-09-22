LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number N=105554676553297;
PocklingtonLehmer(N);
list L=primList(1000);
PocklingtonLehmer(N,L);
tst_status(1);$
