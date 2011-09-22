LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=0,(A,B,C),dp;
matrix m[8][6]=
0,0,  0,   0, 2C, 0,
0,0,  0,   0, -4C,A,
A,-C2,0,   B, -A, 0,
0,0,  1/2B,-A,B,  0,
1,0,  0,   0, 0,  0,
0,0,  0,   2A,B,  0,
0,1,  0,   0, 0,  0,
0,0,  1,   0, 0,  0;
print(rm_unitrow(m));
tst_status(1);$
