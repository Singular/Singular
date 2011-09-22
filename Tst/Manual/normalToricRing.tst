LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring  R = 37,(x,y,t),dp;
ideal I = x3,x2y,y3;
normalToricRing(I);
tst_status(1);$
