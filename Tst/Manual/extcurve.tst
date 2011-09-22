LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list C=Adj_div(x5+y2+y);
C=NSplaces(1..4,C);
// since we have all points up to degree 4, we can extend the curve
// to that extension, in order to get rational points over F_16;
C=extcurve(4,C);
// e.g., display the basepoint of place no. 32:
def R=C[1][5];
setring R;
POINTS[32];
printlevel=plevel;
tst_status(1);$
