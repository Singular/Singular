LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y),dp;
ring R = freeAlgebra(r, 5);
bracket(x,y);
bracket(x,y,2);
tst_status(1);$
