LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
// hyperelliptic curve y^2 + h*y = f
poly h=x;
poly f=x5+5x4+6x2+x+3;
// two divisors
list p=list(-1,-3,1),list(1,1,2);
list q=list(1,1,1),list(2,2,1);
gcddivisor(p,q);
tst_status(1);$
