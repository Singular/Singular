LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list C=Adj_div(x3y+y3+x);
C=NSplaces(1..4,C);
// the first 3 Places in C[3] are of degree 1.
// we define the rational divisor G = 4*C[3][1]+4*C[3][3] (of degree 8):
intvec G=4,0,4;
def R=C[1][2];
setring R;
list LG=BrillNoether(G,C);
// here is the vector basis of L(G):
LG;
printlevel=plevel;
tst_status(1);$
