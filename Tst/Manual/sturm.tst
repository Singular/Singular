LIB "tst.lib"; tst_init();
LIB "rootsur.lib";
ring r = 0,x,dp;
poly p = (x+2)*(x-1)*(x-5);
sturm(p,-3,6);
p = p*(x2+1);
sturm(p,-3,6);
p = p*(x+2);
sturm(p,-3,6);
tst_status(1);$
