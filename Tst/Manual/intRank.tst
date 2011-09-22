LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[3][4] =
1,0,1,0,
1,2,0,0,
0,0,0,0;
int r = intRank(A);
print(A);
print(r); // Should be 2
// another example
intmat B[2][2] =
1,2,
1,2;
int d = intRank(B);
print(B);
print(d); // Should be 1
kill A, B, r, d;
tst_status(1);$
