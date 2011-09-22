LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
// hyperelliptic curve y^2 + h*y = f
poly h=x;
poly f=x5+5x4+6x2+x+3;
poly a=x2+1;
poly b=x;
norm(a,b,h,f);
tst_status(1);$
