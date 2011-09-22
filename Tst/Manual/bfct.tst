LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
bfct(f);
intvec v = 3,2;
bfct(f,1,0,v);
tst_status(1);$
