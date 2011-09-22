LIB "tst.lib"; tst_init();
LIB "primitiv.lib";
ring exring=3,(x,y),dp;
ideal i=x2+1,y3+y2-1;
primitive_extra(i);
ring extension=(3,y),x,dp;
minpoly=y6-y5+y4-y3-y-1;
number a=y5+y4+y2+y+1;
a^2;
factorize(x2+1);
factorize(x3+x2-1);
tst_status(1);$
