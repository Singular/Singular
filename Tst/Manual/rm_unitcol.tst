LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=0,(A,B,C),dp;
matrix m[6][8]=
0,  0,    A,   0, 1,0,  0,0,
0,  0,  -C2,   0, 0,0,  1,0,
0,  0,    0,1/2B, 0,0,  0,1,
0,  0,    B,  -A, 0,2A, 0,0,
2C,-4C,  -A,   B, 0,B,  0,0,
0,  A,    0,   0, 0,0,  0,0;
print(rm_unitcol(m));
tst_status(1);$
