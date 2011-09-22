LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,(x,y),dp;
powerX(100,2,std(ideal(x3-1,y2-x)));
tst_status(1);$
