LIB "tst.lib";
tst_init();

// conversion of bigint to extensions
bigint b=2;
ring r1=(17,a),x,dp;
number n=a+1;
n*b;
ring r2=(0,a),x,dp;
number n=a+1;
n*b;
ring r3=(17,a),x,dp;
minpoly=a2+1;
number n=a+1;
n*b;
ring r4=(0,a),x,dp;
minpoly=a2+1;
number n=a+1;
n*b;

tst_status(1);$
