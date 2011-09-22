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
print(colred(m));"";
list L=colred(m,1);
print(L[1]);
print(L[2]);
tst_status(1);$
