LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,w),dp;
intmat V[2][5]=0,1,2,3,4, -1,1,2,1,3;
intersectionValRingIdeals(V);
tst_status(1);$
