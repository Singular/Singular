LIB "tst.lib";
tst_init();

ring R=0,x,dp;
number n = 2;
number m =1;
diff(n,m);

ring R=(0,a),x,dp;
minpoly = a^2+1;
number n = a;
number m =a;
diff(n,m);

ring R=101,x,dp;
number n = 2;
number m =1;
diff(n,m);

ring R=(101,a),x,dp;
number n = 2;
number m =1;
diff(n,m);

ring R=(0,t1,t2),x,dp;
number n = t1^2*t2^2/(t1-t2)^3;
diff(n,t1^2);

diff(n,t1);
diff(n,t2);
diff(n,0);
number m = 1;
diff(n,m);

tst_status(1);
$

