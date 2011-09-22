LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,w),dp;
intmat V0[2][4]=0,1,2,3, -1,1,2,1;
intersectionValRings(V0);
tst_status(1);$
