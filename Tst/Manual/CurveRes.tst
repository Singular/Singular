LIB "tst.lib"; tst_init();
LIB "curvepar.lib";
ring r=0,(x,y,z),dp;
ideal i=x2-y3,z2-y5;
def s=CurveRes(i);
setring s;
Resolve;
tst_status(1);$
