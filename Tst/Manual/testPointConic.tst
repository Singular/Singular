LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0, (x,y,z), dp;
system("random", 4711);
poly p = x^2 + 2*y^2 + 5*z^2 - 4*x*y + 3*x*z + 17*y*z;
def S = rationalPointConic(p);
if (testPointConic(p, S) == 1)
{ "point lies on conic"; }
else
{ "point does not lie on conic"; }
tst_status(1);$
