LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring r=0,(x,y),dp;
ideal i=y^9 - x^2*(x - 1)^9;
genus(i);
ring r7=7,(x,y),dp;
ideal i=y^9 - x^2*(x - 1)^9;
genus(i);
tst_status(1);$
