LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
//ring R = 0,(x,y),dp;
intmat S2[3][3]=
0, 2, 3,
0, 1, 1,
3, 0, 2;
intmat S1[3][2]=
0, 6,
0, 2,
3, 4;
isSublattice(S1,S2); // Yes!
intmat S3[3][1] =
0,
0,
1;
not(isSublattice(S3,S2)); // Yes!
tst_status(1);$
