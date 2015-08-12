LIB "tst.lib";
tst_init();

ring r = 0, x, dp;
ideal i1 = x;
ideal i2 = x, x2;
chinrem(list(i2, i1), list(2, 3));

kill r;

ring r = 0, x, dp;
ideal i1 = x;
ideal i2 = x, x2;
chinrem(list(i1, i2), list(2, 3));

chinrem(list(i2, i1), list(2, 3));

tst_status(1);$
