LIB "tst.lib"; tst_init();
LIB "random.lib";
ring  r = 0,(x,y,z),lp;
ideal i = randomBinomial(4,5,6);
i;
tst_status(1);$
