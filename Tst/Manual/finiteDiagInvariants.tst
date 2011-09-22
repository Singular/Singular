LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R = 0,(x,y,z,w),dp;
intmat C[2][5] = 1,1,1,1,5, 1,0,2,0,7;
finiteDiagInvariants(C);
tst_status(1);$
