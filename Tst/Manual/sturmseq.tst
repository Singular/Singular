LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,(z,x),dp;
poly p = x5-3x4+12x3+7x-153;
sturmseq(p);
tst_status(1);$
