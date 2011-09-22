LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),Dp;
poly F = x3-y2;
printlevel = 0;
def A  = annfs(F);
setring A;      // Weyl algebra
LD;             // the annihilator of F
def CA = charVariety(LD);
setring CA; CA; // commutative ring
charVar;
dim(charVar);   // hence I is holonomic
tst_status(1);$
