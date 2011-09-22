LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def D = Weyl(); setring D;
poly F = 3*x^2*Dy+2*y*Dx;
poly G = 2*x*Dx+3*y*Dy+6;
ideal I = F,G;
intvec w1 = -1,-1,1,1;
intvec w2 = -1,-2,1,2;
intvec w3 = -2,-3,2,3;
inForm(I,w1);
inForm(I,w2);
inForm(I,w3);
inForm(F,w1);
tst_status(1);$
