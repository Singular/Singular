LIB "tst.lib"; tst_init();

ring r = (0,a), (x,y), lp;
ideal i = x+a*y, x+y;
std(i);

matrix T;
liftstd(i,T);

tst_status(1);$
