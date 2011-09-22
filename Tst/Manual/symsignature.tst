LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y),dp;
ideal i = x4-y2x,y2-13;
i = std(i);
ideal b = qbase(i);
matrix m = matbil(1,b,i);
symsignature(m);
tst_status(1);$
