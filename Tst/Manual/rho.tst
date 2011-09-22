LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,x,dp;
number b=2;
number y=10;
number p=101;
rho(b,y,p);
tst_status(1);$
