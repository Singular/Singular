LIB "tst.lib";
tst_init();

LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly p = 4/7*x^2 + 12/7*x*y + 16/7*y^2 - 1/5*z^2;
paraPlaneCurve(p);

tst_status(1);$
