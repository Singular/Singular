LIB "tst.lib";
tst_init();

LIB"modstd.lib";

example pTestSB;
example modStd;
example modS;
example modHenselStd;
example deleteUnluckyPrimes;
example isIncluded;
ring r = 0, x(1..5), dp;
ideal I = cyclic(5);
ideal I2 = modStd(I);
I2;

tst_status(1);$
