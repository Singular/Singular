LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def R = Weyl();    setring R; // Weyl algebra in variables x,y,Dx,Dy
poly F = x2-y3;
ideal I = (y^3 - x^2)*Dx - 2*x, (y^3 - x^2)*Dy + 3*y^2; // I = Dx*F, Dy*F;
// moreover I is not holonomic, since its dimension is not 2 = 4/2
gkdim(I); // 3
def W = SDLoc(I,F);  setring W; // creates ideal LD in W = R[s]
def U = DLoc0(LD, x2-y3);  setring U; // compute in R
LD0; // Groebner basis of the presentation of localization
BS; // description of b-function for localization
tst_status(1);$
