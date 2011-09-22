LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z,w),dp;
intmat E[2][4] = -1,-1,2,0, 1,1,-2,-1;
torusInvariants(E);
tst_status(1);$
