LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y),Dp;
poly F = x4+y5+x*y4;
printlevel = 0;
def A  = Sannfslog(F);
setring A;
LD1;
tst_status(1);$
