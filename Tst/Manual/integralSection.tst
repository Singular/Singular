LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat P[2][4] =
1,3,4,6,
2,4,5,7;
// should be a matrix with two columns
// for example: [-2, 1, 0, 0], [3, -3, 0, 1]
intmat U = integralSection(P);
print(U);
print(P * U);
kill U;
tst_status(1);$
