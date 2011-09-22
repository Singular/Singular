LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat L[3][3] =
1,4,8,
2,5,10,
3,6,12;
intmat B = latticeBasis(L);
print(B); // should result in a matrix whose columns generate the same lattice as  [1,2,3] and [0,3,6]:
// another example
intmat C[2][4] =
1,1,2,0,
2,3,4,0;
// should result in a matrix whose
// colums create the same  lattice as
// [0,1],[1,0]
intmat D = latticeBasis(C);
print(D);
kill B,L;
tst_status(1);$
