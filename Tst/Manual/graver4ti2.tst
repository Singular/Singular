LIB "tst.lib"; tst_init();
LIB "sing4ti2.lib";
ring r=0,(x,y,z,w),dp;
matrix M[2][4]=0,1,2,3,3,2,1,0;
graver4ti2(M);
tst_status(1);$
