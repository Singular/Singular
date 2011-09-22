LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,x,dp;
number n = 4/3;
isInt(n);
n = 11;
isInt(n);
tst_status(1);$
