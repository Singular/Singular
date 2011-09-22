LIB "tst.lib"; tst_init();
LIB "involut.lib";
ring R = 0,(x,d),dp;
def r = nc_algebra(1,1); setring r; // Weyl-Algebra
map F = r,x,-d;
F(F);  // should be maxideal(1) for an involution
poly f =  x*d^2+d;
poly If = involution(f,F);
f-If;
poly g = x^2*d+2*x*d+3*x+7*d;
poly tg = -d*x^2-2*d*x+3*x-7*d;
poly Ig = involution(g,F);
tg-Ig;
ideal I = f,g;
ideal II = involution(I,F);
II;
matrix(I) - involution(II,F);
module M  = [f,g,0],[g,0,x^2*d];
module IM = involution(M,F);
print(IM);
print(matrix(M) - involution(IM,F));
tst_status(1);$
