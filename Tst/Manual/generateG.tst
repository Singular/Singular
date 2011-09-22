LIB "tst.lib"; tst_init();
LIB "crypto.lib";
ring R = 0,(x,y),dp;
generateG(7,15,4);
tst_status(1);$
