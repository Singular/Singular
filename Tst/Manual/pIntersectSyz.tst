LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
def D = initialMalgrange(f);
setring D;
inF;
poly s = t*Dt;
pIntersectSyz(s,inF);
int p = prime(20000);
pIntersectSyz(s,inF,p,0,0);
tst_status(1);$
