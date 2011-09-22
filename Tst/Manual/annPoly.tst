LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,z),dp;
poly f = x^2*z - y^3;
def A = annPoly(f);
setring A;    // A is the 3rd Weyl algebra in 6 variables
LD;           // the Groebner basis of annihilator
gkdim(LD);    // must be 3 = 6/2, since A/LD is holonomic module
NF(Dy^4, LD); // must be 0 since Dy^4 clearly annihilates f
poly f = imap(r,f);
NF(LD*f,std(ideal(Dx,Dy,Dz))); // must be zero if LD indeed annihilates f
tst_status(1);$
