LIB "tst.lib"; tst_init();
LIB "gmspoly.lib";
ring R=0,(x,y),dp;
isTame(x2y+x);
isTame(x3+y3+xy);
tst_status(1);$
