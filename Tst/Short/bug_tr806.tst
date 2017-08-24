LIB "tst.lib";
tst_init();

LIB "absfact.lib";
ring r0 = 0,(x,y),dp;
poly H = x^3-21*x^2*y-42*x*y^2-19*y^3+3*x^2+12*x*y+12*y^2+9*x+18*y+27;
def S = absFactorizeBCG(H); setring S; absolute_factors;
setring r0;
kill S;
def S = absFactorize(H); setring S; absolute_factors;

tst_status(1);$
