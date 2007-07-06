LIB "tst.lib";
tst_init();

====================================== example B.6.4 ====================

ring R=0,(x,y),dp;
poly f=x2-(y+1)^3;
factorize(f);

tst_status(1);$

