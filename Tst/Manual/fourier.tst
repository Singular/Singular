LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def D2 = Weyl();
setring D2;
ideal I = x*Dx+2*y*Dy+2, x^2*Dx+y*Dx+2*x;
intvec v = 2;
fourier(I,v);
fourier(I);
tst_status(1);$
