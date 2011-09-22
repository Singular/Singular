LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=(0,a,b),(A,B,C),dp;
matrix m[8][6]=
0,    2*C, 0,    0,  0,   0,
0,    -4*C,a*A,  0,  0,   0,
b*B,  -A,  0,    0,  0,   0,
-A,   B,   0,    0,  0,   0,
-4*C, 0,   B,    2,  0,   0,
2*A,  B,   0,    0,  0,   0,
0,    3*B, 0,    0,  2b,  0,
0,    AB,  0,    2*A,A,   2a;"";
list L=gauss_col(m,1);
print(L[1]);
print(L[2]);
ring S=0,x,(c,dp);
matrix A[5][4] =
3, 1, 1, 1,
13, 8, 6,-7,
14,10, 6,-7,
7, 4, 3,-3,
2, 1, 0, 3;
print(gauss_col(A));
tst_status(1);$
