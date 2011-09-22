LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring r=0,(x,y,z),dp;
ideal i=1+x+x2+x3,3,x+3y+5z;
linearpart(i);
module m=[x,y,z],x*[x3,y2,z],[1,x2,z3,0,1];
show(linearpart(m));
tst_status(1);$
