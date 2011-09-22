LIB "tst.lib"; tst_init();
LIB "bfun.lib";
intvec v = 1,2,3;
allPositive(v);
intvec w = 1,-2,3;
allPositive(w);
tst_status(1);$
