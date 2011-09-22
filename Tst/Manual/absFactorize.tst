LIB "tst.lib"; tst_init();
LIB "absfact.lib";
ring R = (0), (x,y), lp;
poly p = (-7*x^2 + 2*x*y^2 + 6*x + y^4 + 14*y^2 + 47)*(5x2+y2)^3*(x-y)^4;
def S = absFactorize(p) ;
setring(S);
absolute_factors;
tst_status(1);$
