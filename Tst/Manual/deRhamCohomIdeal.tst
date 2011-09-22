LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z),dp;
poly F = x^3+y^3+z^3;
bfctAnn(F);            // Bernstein-Sato poly of F has minimal integer root -2
def W = annRat(1,F^2); // so we compute the annihilator of 1/F^2
setring W; W;          // Weyl algebra, contains LD = Ann(1/F^2)
LD;                    // K[x,y,z,1/F]F^(-2) is isomorphic to W/LD as W-module
deRhamCohomIdeal(LD);  // we see that the K-dim is 2
tst_status(1);$
