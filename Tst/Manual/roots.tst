LIB "tst.lib"; tst_init();
LIB "zeroset.lib";
ring R = (0,a), x, lp;
minpoly = a2+1;
poly f = x3 - a;
def R1 = roots(f);
setring R1;
minpoly;
newA;
f;
theRoots;
map F;
F[1] = theRoots[1];
F(f);
tst_status(1);$
