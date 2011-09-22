LIB "tst.lib"; tst_init();
LIB "solve.lib";
// compute the intersection points of two curves
ring rsq = 0,(x,y),lp;
ideal gls=  x2 + y2 - 10, x2 + xy + 2y2 - 16;
def R=ures_solve(gls,0,16);
setring R; SOL;
tst_status(1);$
