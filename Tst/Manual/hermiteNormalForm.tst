LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat M[2][5] =
1, 2, 3, 4, 0,
0,10,20,30, 1;
// Hermite Normal Form of M:
print(hermiteNormalForm(M));
intmat T[3][4] =
3,3,3,3,
2,1,3,0,
1,2,0,3;
// Hermite Normal Form of T:
print(hermiteNormalForm(T));
intmat A[4][5] =
1,2,3,2,2,
1,2,3,4,0,
0,5,4,2,1,
3,2,4,0,2;
// Hermite Normal Form of A:
print(hermiteNormalForm(A));
tst_status(1);$
