LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def R = Weyl();    setring R; // Weyl algebra in variables x,y,Dx,Dy
poly F = x2-y3;
ideal I = (y^3 - x^2)*Dx - 2*x, (y^3 - x^2)*Dy + 3*y^2; // I = Dx*F, Dy*F;
// I is not holonomic, since its dimension is not 4/2=2
gkdim(I);
list L = DLoc(I, x2-y3);
L[1]; // localized module (R/I)_f is isomorphic to R/LD0
L[2]; // description of b-function for localization
tst_status(1);$
