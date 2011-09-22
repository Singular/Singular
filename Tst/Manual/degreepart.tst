LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring r=0,(x,y,z),dp;
ideal i=1+x+x2+x3+x4,3,xz+y3+z8;
degreepart(i,0,4);
module m=[x,y,z],x*[x3,y2,z],[1,x2,z3,0,1];
intvec v=2,3,6;
show(degreepart(m,8,8,v));
tst_status(1);$
