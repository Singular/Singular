LIB "tst.lib";
tst_init();

LIB"curvepar.lib";
ring R=0,(x,y),ds;
ideal I=y;
def S=CurveParam(I);
setring S;
Param;

tst_status(1);$
