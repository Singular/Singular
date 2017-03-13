LIB "tst.lib";
tst_init();

// intvec uninitialized for thise exceptional case
ring r = 0, x, dp;
ideal i = 1;
indepSet(std(i));

tst_status(1);$
