LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number h=10;
number p=h^100+267;
//p=h^100+43723;
//p=h^200+632347;
SolowayStrassen(h,3);
tst_status(1);$
