LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring r  = 0,(x,y,z),dp;
ideal i = y2-x3,x-3,y-2x;
def R_r = tolessvars(i,"lp");
setring R_r;
show(basering);
IMAG;
kill R_r;
tst_status(1);$
