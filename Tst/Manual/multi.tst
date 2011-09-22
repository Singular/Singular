LIB "tst.lib"; tst_init();
LIB "hyperel.lib";
ring R=7,x,dp;
poly h=x;
poly f=x5+5x4+6x2+x+3;
// hyperelliptic curve y^2 + h*y = f
poly a=x2+1;
poly b=x;
poly c=5;
poly d=-x;
multi(a,b,c,d,h,f);
tst_status(1);$
