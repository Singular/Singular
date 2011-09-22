LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly f = y^8-x^3*(z+x)^5;
ideal adj = adjointIdeal(f);
def Rn = invertBirMap(adj,ideal(f));
setring(Rn);
J;
def Rc = rncItProjEven(J);
PHI;
setring Rc;
CONIC;
def RPc = paraConic(CONIC);
setring RPc;
PARACONIC;
tst_status(1);$
