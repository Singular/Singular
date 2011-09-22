LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring R = 0,(x,y),dp;
poly F = x*y*(x+y);
def A = annfsBM(F,0);
setring A;
LD;
isHolonomic(LD);
ideal I = std(LD[1]);
I;
isHolonomic(I);
tst_status(1);$
