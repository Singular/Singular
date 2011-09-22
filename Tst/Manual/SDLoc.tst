LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def R = Weyl(); // Weyl algebra on the variables x,y,Dx,Dy
setring R;
poly F = x2-y3;
ideal I = Dx*F, Dy*F;
// note, that I is not holonomic, since it's dimension is not 2
gkdim(I);  // 3, while dim R = 4
def W = SDLoc(I,F);
setring W; // = R[s], where s is a new variable
LD;        // Groebner basis of s-parametric presentation
tst_status(1);$
