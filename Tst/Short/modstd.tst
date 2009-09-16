LIB "tst.lib";
tst_init();

LIB"modstd.lib";

example pTestSB;
example primList;
example modStd;
example modS;
example deleteUnluckyPrimes;
example liftPoly;
example liftPoly1;
example Farey;
example chineseR; 
example pStd;
example transmat;
example fareyMatrix;
example MmodN;
ring r = 0, x(1..5), dp;
ideal I = cyclic(5);
ideal I2 = modStd(I);
I2;

tst_status(1);$
