LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y),dp;
ideal i = x4-y2x,y2-13;
i = std(i);
ideal b = qbase(i);
sturmquery(1,b,i);
tst_status(1);$
