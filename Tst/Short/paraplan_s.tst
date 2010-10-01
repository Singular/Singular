LIB "tst.lib";
tst_init();

/////////////////////////////////////////////////////////////////////////////
/// Further examples for testing the main procedures
/// Timings on wawa Sept 29
/////////////////////////////////////////////////////////////////////////////
LIB"paraplanecurves.lib";
// -------------------------------------------------------
// Example 1
// -------------------------------------------------------
ring RR = 0, (x,y,z), dp;
poly f = 7*z2+11*y2+13*z*y+17*x2+19*x*y; // conic
def RP1 = paraConic(f);
setring RP1; PARACONIC;
setring RR;
RP1 = paraPlaneCurve(f);
testParametrization(f,RP1);
setring RP1; PARA;
kill RR;kill RP1;

tst_status(1);$

