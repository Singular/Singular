LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
// hyperelliptic curve y^2 + h*y = f
poly h=x;
poly f=x5+5x4+6x2+x+3;
// semireduced divisor
list D=2x4+3x3-3x-2, -x3-2x2+3x+1;
cantorred(D,h,f);
tst_status(1);$
