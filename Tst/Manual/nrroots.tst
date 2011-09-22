LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
poly p = (x+2)*(x-1)*(x-5);
nrroots(p);
p = p*(x2+1);
nrroots(p);
tst_status(1);$
