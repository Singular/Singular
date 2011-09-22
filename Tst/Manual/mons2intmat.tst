LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z),dp;
ideal I=x2,y2,x2yz3;
mons2intmat(I);
tst_status(1);$
