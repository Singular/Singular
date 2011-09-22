LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[3][3] =
1,1,3,
3,2,0,
0,0,1;
intmat B = intInverse(A);
// should be the unit matrix
print(A * B);
// another example
intmat C[2][2] =
2,1,
3,2;
intmat D  = intInverse(C);
// should be the unit matrix
print(C * D);
kill A, B, C, D;
tst_status(1);$
