LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=0,(x,y,z,a,b),dp;
ideal i=ax2+ay3-b2x,abz+by2;
valvars (i,0,xyz);
tst_status(1);$
