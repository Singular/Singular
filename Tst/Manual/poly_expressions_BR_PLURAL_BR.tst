LIB "tst.lib"; tst_init();
ring r=0,(x,y),dp;
def R=nc_algebra(1,1);  // make it a Weyl algebra
setring R;
R;
yx;      // not correct input
y*x;     // correct input
poly f = 10x2*y3 + 2y2*x^2 - 2*x*y + y - x + 2;
lead(f);
leadmonom(f);
simplify(f,1);     // normalize leading coefficient
cleardenom(f);
tst_status(1);$
