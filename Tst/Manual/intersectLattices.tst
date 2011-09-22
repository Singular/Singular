LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[3][2] =
1,4,
2,5,
3,6;
intmat B[3][2] =
6,9,
7,10,
8,11;
// should result in a (3x2)-matrix with columns
//  e.g. [0, 3, 6], [-3, 0, 3] (the lattice should be the same)
print(intersectLattices(A,B));
// another example
intmat C[2][3] =
1,0,0,
3,2,5;
intmat D[2][3] =
4,5,0,
0,5,0;
// should result in a (3x2)-matrix whose columns generate the
// same lattice as [1,5], [0, 20]
print(intersectLattices(C,D));
tst_status(1);$
