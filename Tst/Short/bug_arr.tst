LIB "tst.lib";
tst_init();

LIB"arr.lib";
ring r;
arr A=x;
A==A;

tst_status(1);$
