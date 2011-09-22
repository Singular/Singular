LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring s  = 23,(x,y),dp;
list L = (x-y),(x3+y2);
iMult(L);
L = (x-y),(x3+y2),(x3-y4);
iMult(L);
tst_status(1);$
