LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,t),dp;
ideal I=x^2,y^2,z^3;
list l=intclMonIdeal(I);
showNuminvs();
tst_status(1);$
