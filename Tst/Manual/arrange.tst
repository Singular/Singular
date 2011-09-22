LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring X = 0,(x,y,z,t),dp;
poly q = arrange(3);
factorize(q,1);
tst_status(1);$
