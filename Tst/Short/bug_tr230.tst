LIB "tst.lib";
tst_init();

// fglm of the 0-ideal should return an error:
ring r = 0,(x, y), dp;
ideal i;
fglm(r,i);

tst_status(1);$
