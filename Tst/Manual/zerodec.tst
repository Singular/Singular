LIB "tst.lib"; tst_init();
LIB "primdec.lib";
ring r  = 0,(x,y),dp;
ideal i = x2-2,y2-2;
list pr = zerodec(i);
pr;
tst_status(1);$
