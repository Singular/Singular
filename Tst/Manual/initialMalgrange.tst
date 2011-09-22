LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
def D = initialMalgrange(f);
setring D;
inF;
setring r;
intvec v = 3,2;
def D2 = initialMalgrange(f,1,1,v);
setring D2;
inF;
tst_status(1);$
