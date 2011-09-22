LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
// hyperelliptic curve y^2 + h*y = f
poly h=x;
poly f=x5+5x4+6x2+x+3;
// Divisor
list D=x2-1,2x-1;
semidiv(D,h,f);
tst_status(1);$
