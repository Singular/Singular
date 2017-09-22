LIB "tst.lib";
tst_init();

// warn, if prime is too large for factory:
ring r=536871001,x,dp;
factorize (x2-1);

tst_status(1);$
