LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list C=Adj_div(x3y+y3+x);
// The list of computed places:
C[3];
// create places up to degree 4
list L=NSplaces(1..4,C);
// The list of computed places is now:
L[3];
// e.g., affine non-singular points of degree 4 :
def aff_r=L[1][1];
setring aff_r;
Aff_Points(4);
// e.g., base point of the 1st place of degree 4 :
def S(4)=L[5][4][1];
setring S(4);
POINTS[1];
printlevel=plevel;
tst_status(1);$
