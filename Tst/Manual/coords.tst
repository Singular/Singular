LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y),dp;
ideal i = x4-y2x,y2-13;
poly f = x3-xy+y-13+x4-y2x;
i = std(i);
ideal b = qbase(i);
b;
coords(f,b,i);
tst_status(1);$
