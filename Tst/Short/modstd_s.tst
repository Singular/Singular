LIB "tst.lib";
tst_init();

LIB "modstd.lib";

example modStd;

/* standard: cyclic 5 */
ring r1 = 0, x(1..5), dp;
ideal I = cyclic(5);
modStd(I);

/* without final tests */
ring r2 = 0, x(1..6), dp;
ideal I = cyclic(6);
modStd(I, 0);

/* with parameters */
ring r3 = (0,u,v,w), (x,y,z), dp;
ideal I = x+u*y, y+v*z;
modStd(I);

/* for primeTest_std() */
ring r4 = 0, x, dp;
ideal I = 2147483587*x2+1;
modStd(I);

/* for deleteUnluckyPrimes_std() */
ring r5 = 0, (x,y), dp;
ideal I = 2*x+y, x+2^30*y;
modStd(I);

tst_status(1);$
