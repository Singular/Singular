LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
int q=32; int n=25; int redun=n-11; int t=redun+1;
ring r=(q,a),x,dp;
// correct 2 errors in 2 random binary codes, 3 trials each
decodeRandom(n,redun,2,3,2);
tst_status(1);$
