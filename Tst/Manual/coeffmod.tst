LIB "tst.lib"; tst_init();
LIB "aksaka.lib";
ring R = 0,x,dp;
poly f=2457*x4+52345*x3-98*x2+5;
number n=3;
coeffmod(f,n);
tst_status(1);$
