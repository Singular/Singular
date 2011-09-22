LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=0,(u,x,y,z),dp;
ideal i = u3+y3+z-x,x2y2+z3,y+z+1,y+u;
elimlinearpart(i);
tst_status(1);$
