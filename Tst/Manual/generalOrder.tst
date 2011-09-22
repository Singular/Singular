LIB "tst.lib"; tst_init();
LIB "weierstr.lib";
ring R = 0,(x,y),ds;
poly f = x2-4xy+4y2-2xy2+4y3+y4;
generalOrder(f);
tst_status(1);$
