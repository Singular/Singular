LIB "tst.lib";
tst_init();

LIB "matrix.lib";

ring r = (2,t),(x),dp;
matrix m[1][1] = x+t;
primdecSY(m);

tst_status(1);$
