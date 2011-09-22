LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list C=Adj_div(x3y+y3+x);
C=NSplaces(1..4,C);
def R=C[1][2];
setring R;
// Place C[3][1] has degree 1 (i.e it is rational);
list WS=Weierstrass(1,7,C);
// the first part of the list is the Weierstrass semigroup up to 7 :
WS[1];
// and the second part are the corresponding functions :
WS[2];
printlevel=plevel;
tst_status(1);$
