LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=0,(x,y,z),dp;
ideal i = x3+y2+z,x2y2+z3,y+z+1;
elimpartanyr(i,z);
tst_status(1);$
