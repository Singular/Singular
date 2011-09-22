LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[3][2] =
1,4,
2,5,
3,6;
// should be 0
int b = isPrimitiveSublattice(A);
print(b);
// another example
intmat B[2][2] =
1,0,
0,1;
// should be 1
int c = isPrimitiveSublattice(B);
print(c);
kill A, b, B, c;
tst_status(1);$
