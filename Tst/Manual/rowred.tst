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
print(rowred(m));"";
list L=rowred(m,1);
print(L[1]);
print(L[2]);
tst_status(1);$
