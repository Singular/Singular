LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,b,Dx,Db),dp;
def D2 = Weyl();
setring D2;
ideal I = x*Dx+2*b*Db+2, x^2*Dx+b*Dx+2*x;
intvec w = 1,0;
def D1 = integralIdeal(I,w);
setring D1; D1;
intIdeal;
tst_status(1);$
