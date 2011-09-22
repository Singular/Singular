LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
poly f = x^2+y^3+x*y^2;
bfctAnn(f);
def R = reiffen(4,5); setring R;
RC; // the Reiffen curve in 4,5
bfctAnn(RC,0,1);
tst_status(1);$
