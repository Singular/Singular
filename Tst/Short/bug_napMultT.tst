LIB "tst.lib";
tst_init();

// this was a bug in napMultT due to incorrect handling of
// nacNeg 
option(prot);
LIB "hnoether.lib";
ring r=(0,i),(a,b),dp;
minpoly=i2+1;
develop(a2+ib3);
develop(a5+i2*a4+2i*a2b+b2);
develop((a+2b)^2+ib5);

tst_status(1);$
