LIB "tst.lib"; tst_init();
LIB "solve.lib";
ring r = 0,(x,y),lp;
// compute the intersection points of two curves
ideal s=  x2 + y2 - 10, x2 + xy + 2y2 - 16;
def R=triang_solve(triangLfak(stdfglm(s)),10);
setring R; rlist;
tst_status(1);$
