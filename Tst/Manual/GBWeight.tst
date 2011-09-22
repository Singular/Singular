LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y,Dx,Dy),dp;
def D2 = Weyl();
setring D2;
ideal I = 3*x^2*Dy+2*y*Dx,2*x*Dx+3*y*Dy+6;
intvec u = -2,-3;
intvec v = -u;
GBWeight(I,u,v);
ideal J = std(I);
GBWeight(J,u,v); // same as above
u = 0,1;
GBWeight(I,u,v);
tst_status(1);$
