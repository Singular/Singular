LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly f = y^8-x^3*(z+x)^5;
def RP1 = paraPlaneCurve(f);
testParametrization(f, RP1);
tst_status(1);$
