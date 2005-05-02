LIB "tst.lib";
tst_init();

LIB"solve.lib";
ring r=0,(x,y,z),lp;
ideal i=x^2-2,z-1,y;
def A=fglm_solve(i);
setring A;
rlist;

//
tst_status(1);$
