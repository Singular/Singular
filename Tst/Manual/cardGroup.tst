LIB "tst.lib"; tst_init();
LIB "cisimplicial.lib";
intmat A[3][5] = 24,  0,  0,  8, 3,
0, 24,  0, 10, 6,
0,  0, 24,  5, 9;
cardGroup(A);
tst_status(1);$
