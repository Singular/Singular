LIB "tst.lib"; tst_init();
LIB "signcond.lib";
ring r = 0,(x,y),dp;
ideal i = (x-2)*(x+3)*x,y*(y-1);
firstoct(i);
tst_status(1);$
