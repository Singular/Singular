LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat B[4][2] =
1,5,
2,6,
3,7,
4,8;
// should result in a (2x4)-matrix such that the corresponding lattice is created by
// [-1, 2], [-2, 3], [-1, 0] and [0, 1]
print(projectLattice(B));
// another example
intmat BB[4][2] =
1,0,
0,1,
0,0,
0,0;
// should result in a (2x4)-matrix such that the corresponding lattice is created by
// [0,0],[0,0],[1,0],[0,1]
print(projectLattice(BB));
// another example
intmat BBB[3][4] =
1,0,1,2,
1,1,0,0,
3,0,0,3;
// should result in the (1x3)-matrix that consists of just zeros
print(projectLattice(BBB));
tst_status(1);$
