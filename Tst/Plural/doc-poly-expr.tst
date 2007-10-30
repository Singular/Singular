LIB "tst.lib";
tst_init();
ring r=0,(x,y),dp;
poly f = 10x2y3 +2x2y2-2xy+y -x+2;
lead(f);
leadmonom(f);
simplify(f,1);     // normalize leading coefficient
poly g = 1/2x2 + 1/3y;
cleardenom(g);
int i = 102;
poly(i);
typeof(_);
kill r;
ring r=0,(x,y),dp;
def S=ncalgebra(1,1); setring S;
// it is a Weyl algebra 
S;
yx;      // not correct input
y*x;     // correct input
tst_status(1);$
