LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly f = y^8-x^3*(z+x)^5;
ideal adj = adjointIdeal(f);
def Rn = mapToRatNormCurve(f,adj);
setring(Rn);
RNC;
rncAntiCanonicalMap(RNC);
tst_status(1);$
