LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,z,dp;
number b=2;
number y=10;
number p=101;
babyGiant(b,y,p);
tst_status(1);$
