LIB "tst.lib"; tst_init();
LIB "matrix.lib";
ring r=0,(A,B,C),dp;
matrix M[2][3]=
0,A,  B,
A2, B2, C;
print(M);
print(headStand(M));
tst_status(1);$
