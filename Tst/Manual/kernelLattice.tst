LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat LL[3][4] =
1,4,7,10,
2,5,8,11,
3,6,9,12;
// should be a 4x2 matrix whose columns
// generate the same lattice as [-1,2,-1,0],[2,-3,0,1]
intmat B = kernelLattice(LL);
print(B);
// another example
intmat C[2][4] =
1,0,2,0,
0,1,2,0;
// should result in a matrix whose
// colums create the same  lattice as
// [-2,-2,1,0], [0,0,0,1]
intmat D = kernelLattice(C);
print(D);
kill B;
tst_status(1);$
