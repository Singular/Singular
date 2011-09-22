LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y,z),dp;
ideal i = 2x2,-y2,z3;
i = std(i);
ideal b = qbase(i);
b;
b = kbase(i);
b; // Compare this with the result of qbase
tst_status(1);$
