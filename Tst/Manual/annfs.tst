LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r = 0,(x,y,z),Dp;
poly F = z*x^2+y^3;
def A  = annfs(F); // here, the default BM algorithm will be used
setring A; // the Weyl algebra in (x,y,z,Dx,Dy,Dz)
LD; //the annihilator of F^{-1} over A
BS; // roots with multiplicities of BS polynomial
tst_status(1);$
