LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
ring R=0,(x,y,z),dp;
intmat expo_vecs[3][3] =
2,0,0,
0,2,0,
2,1,3;
intmat2mons(expo_vecs);
tst_status(1);$
