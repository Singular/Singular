LIB "tst.lib"; tst_init();
LIB "elim.lib";
ring r=0,(x,y,u,v,w),dp;
ideal i=x-u,y-u2,w-u3,v-x+y3;
elim(i,3..4);
elim(i,uv);
int p = printlevel;
printlevel = 2;
elim(i,uv,"withWeights","slimgb");
printlevel = p;
ring S =  (0,a),(x,y,z,u,v),ws(1,2,3,4,5);
minpoly = a2+1;
qring T = std(ideal(ax+y2+v3,(x+v)^2));
ideal i=x-u,y-u2,az-u3,v-x+ay3;
module m=i*gen(1)+i*gen(2);
m=elim(m,xy);
show(m);
tst_status(1);$
