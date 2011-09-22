LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r = 0,(x),dp;
matrix A[5][5] = 20,  4,  0, -9,   8,
4, 12, -6, -4,   9,
0, -6, -2, -9,  -8,
-9, -4, -9, -20, 10,
8,  9, -8,  10, 10;
pos_def(A);
matrix B[3][3] =  3,  2,  0,
2, 12,  4,
0,  4,  2;
pos_def(B);
tst_status(1);$
