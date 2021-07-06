LIB "tst.lib"; tst_init();
LIB "realclassify.lib";
ring r = 0,x,dp;
bigint D=-23;
HilbertClassPoly(D,50);
tst_status(1);$
