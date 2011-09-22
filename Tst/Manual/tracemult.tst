LIB "tst.lib"; tst_init();
LIB "rootsmr.lib";
ring r = 0,(x,y),dp;
ideal i = x4-y2x,y2-13;
i = std(i);
ideal b = qbase(i);
poly f = x3-xy+y-13+x4-y2x;
matrix m = matmult(f,b,i);
print(m);
tracemult(f,b,i);            //the trace of m
tst_status(1);$
