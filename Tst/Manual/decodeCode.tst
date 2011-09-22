LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
int q=32; int n=25; int redun=n-11; int t=redun+1;
ring r=(q,a),x,dp;
matrix check=randomCheck(redun,n,1);
// correct 2 errors in using the code above, 3 trials
decodeCode(check,3,2);
tst_status(1);$
