LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat Q[2][3] =
1,2,3,
3,2,1;
intmat L[3][2] =
1,4,
2,5,
3,6;
// should be a 2x2 matrix with columns
// [2,-14], [0,36]
imageLattice(Q,L);
tst_status(1);$
