LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0, (x,y,z), dp;
system("random", 4711);
poly p = x^2 + 2*y^2 + 5*z^2 - 4*x*y + 3*x*z + 17*y*z;
def S = rationalPointConic(p); // quadratic field extension,
// minpoly = a^2 - 2
testPointConic(p, S);
setring R;
p = x^2 - 1857669520 * y^2 + 86709575222179747132487270400 * z^2;
S = rationalPointConic(p); // same as current basering,
// no extension needed
testPointConic(p, S);
tst_status(1);$
