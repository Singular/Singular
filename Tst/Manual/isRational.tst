LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = (0,a),(x,y),dp;
number n1 = 11/73;
isRational(n1);
number n2 = (11*a+3)/72;
isRational(n2);
tst_status(1);$
