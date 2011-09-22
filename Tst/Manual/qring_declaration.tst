LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),dp;
ideal i=xy;
qring q=std(i);
basering;
// simplification is not immediate:
(x+y)^2;
reduce(_,std(0));
tst_status(1);$
