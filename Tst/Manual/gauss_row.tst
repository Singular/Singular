LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=(0,a,b),(A,B,C),dp;
matrix m[6][8]=
0, 0,  b*B, -A,-4C,2A,0, 0,
2C,-4C,-A,B, 0,  B, 3B,AB,
0,a*A,  0, 0, B,  0, 0, 0,
0, 0,  0, 0, 2,  0, 0, 2A,
0, 0,  0, 0, 0,  0, 2b, A,
0, 0,  0, 0, 0,  0, 0, 2a;"";
print(gauss_row(m));"";
ring S=0,x,dp;
matrix A[4][5] =  3, 1,1,-1,2,
13, 8,6,-7,1,
14,10,6,-7,1,
7, 4,3,-3,3;
list L=gauss_row(A,1);
print(L[1]);
print(L[2]);
tst_status(1);$
