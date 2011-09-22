LIB "tst.lib"; tst_init();
LIB "aksaka.lib";
ring R=0,x,dp;
poly a=3*x3-x2+5;
poly r=x7-1;
number q=123;
number n=5;
powerpolyX(q,n,a,r);
tst_status(1);$
