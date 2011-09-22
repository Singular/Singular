LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=37,(x,y,t),dp;
ideal J=x3,x2y,y3,xy2t7;
ehrhartRing(J);
tst_status(1);$
