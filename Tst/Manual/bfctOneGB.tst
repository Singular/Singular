LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
bfctOneGB(f);
bfctOneGB(f,1,1);
tst_status(1);$
