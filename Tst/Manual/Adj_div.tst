LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
int plevel=printlevel;
printlevel=-1;
ring s=2,(x,y),lp;
list C=Adj_div(y9+y8+xy6+x2y3+y2+x3);
def aff_R=C[1][1];      // the affine ring
setring aff_R;
listvar(aff_R);         // data in the affine ring
CHI;                    // affine equation of the curve
Aff_SLocus;             // ideal of the affine singular locus
Aff_SPoints[1];         // 1st affine singular point: (1:1:1), no.1
Inf_Points[1];          // singular point(s) at infinity: (1:0:0), no.4
Inf_Points[2];          // list of non-singular points at infinity
//
def proj_R=C[1][2];     // the projective ring
setring proj_R;
CHI;                    // projective equation of the curve
C[2][1];                // degree of the curve
C[2][2];                // genus of the curve
C[3];                   // list of computed places
C[4];                   // adjunction divisor (all points are singular!)
//
// we look at the place(s) of degree 2 by changing to the ring
C[5][2][1];
def S(2)=C[5][2][1];
setring S(2);
POINTS;                // base point(s) of place(s) of degree 2: (1:a:1)
LOC_EQS;               // local equation(s)
PARAMETRIZATIONS;      // parametrization(s) and exactness
BRANCHES;              // Hamburger-Noether development
printlevel=plevel;
tst_status(1);$
