LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=0,(u,x,y,z),dp;
ideal i = xy2-xu4-x+y2,x2y2+z3+zy,y+z2+1,y+u2;
elimpart(i);
i = interred(i); i;
elimpart(i);
elimpart(i,2);
tst_status(1);$
