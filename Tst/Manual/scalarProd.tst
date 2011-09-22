LIB "tst.lib"; tst_init();
LIB "bfun.lib";
intvec v = 1,2,3;
intvec w = 4,5,6;
scalarProd(v,w);
tst_status(1);$
