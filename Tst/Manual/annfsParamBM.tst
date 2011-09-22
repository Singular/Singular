LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = (0,a,b),(x,y),Dp;
poly F = x^2 - (y-a)*(y-b);
printlevel = 0;
def A = annfsParamBM(F);  setring A;
LD;
Param;
setring r;
poly G = x2-(y-a)^2; // try the exceptional value b=a of parameters
def B = annfsParamBM(G); setring B;
LD;
Param;
tst_status(1);$
