LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),dp;
poly g = 2*x*y;  poly f = x^2 - y^3;
def B = annRat(g,f);
setring B;
LD;
// Now, compare with the output of Macaulay2:
ideal tst = 3*x*Dx + 2*y*Dy + 1, y^3*Dy^2 - x^2*Dy^2 + 6*y^2*Dy + 6*y,
9*y^2*Dx^2*Dy-4*y*Dy^3+27*y*Dx^2+2*Dy^2, 9*y^3*Dx^2-4*y^2*Dy^2+10*y*Dy -10;
option(redSB); option(redTail);
LD = groebner(LD);
tst = groebner(tst);
print(matrix(NF(LD,tst)));  print(matrix(NF(tst,LD)));
// So, these two answers are the same
tst_status(1);$
