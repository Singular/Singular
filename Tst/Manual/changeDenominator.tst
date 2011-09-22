LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring s = 0,(x,y),dp;
ideal I = y5-y4x+4y2x2-x4;
ideal U1 = normal(I)[2][1];
poly c1 = U1[4];
U1;c1;
// 1/c1 * U1 is the normalization of I.
ideal U2 = changeDenominator(U1, c1, x3, I);
U2;
// 1/x3 * U2 is also the normalization of I, but with a different denominator.
tst_status(1);$
