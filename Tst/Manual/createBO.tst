LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y,z),dp;
ideal J=x2-y3;
createBO(J,ideal(z));
tst_status(1);$
