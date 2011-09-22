LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
def D = initialMalgrange(f);
setring D;
inF;
pIntersect(t*Dt,inF);
pIntersect(t*Dt,inF,1);
tst_status(1);$
