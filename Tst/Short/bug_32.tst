LIB "tst.lib";
tst_init();

LIB"solve.lib";
ring r=0,(x,y,z),lp;
ideal i=x^2-2,z-1,y;
fglm_solve(i);
rlist;

//
tst_status(1);$
