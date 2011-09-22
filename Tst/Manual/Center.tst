LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y),dp;
ideal J=x2-y3;
Center(J);
tst_status(1);$
