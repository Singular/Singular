LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),Dp;
poly F = x^3+y^3+z^3;
printlevel = 0;
def A  = Sannfs(F); // here, the default BM algorithm will be used
setring A;
LD;
tst_status(1);$
