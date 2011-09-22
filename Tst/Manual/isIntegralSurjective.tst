LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[2][3] =
1,3,5,
2,4,6;
// should be 0
int b = isIntegralSurjective(A);
print(b);
// another example
intmat B[2][3] =
1,1,5,
2,3,6;
// should be 1
int c = isIntegralSurjective(B);
print(c);
kill A, b, B, c;
tst_status(1);$
