LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly f = -x7-10x5y2-10x4y3-3x3y4+8x2y5+7xy6+11y7+3x6+10x5y +30x4y2
+26x3y3-13x2y4-29xy5-33y6-3x5-20x4y-33x3y2-8x2y3+37xy4+33y5
+x4+10x3y+13x2y2-15xy3-11y4;
f = homog(f,z);
ideal adj = adjointIdeal(f);
def Rn = mapToRatNormCurve(f,adj);
setring(Rn);
RNC;
rncItProjOdd(RNC);
tst_status(1);$
