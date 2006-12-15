LIB "tst.lib";
tst_init();
ring r=0,(x,y,z,u,v,w),dp;
ncalgebra(-1,0);
// anticommutative algebra
option(redSB);
option(redTail);
ideal i=x^2,y^2,z^2,u^2,v^2,w^2;
qring q=system("twostd",i);
// now it is an exterior algebra
q;
poly k=(x+y)*(y-u)*(zv+u-w);
k;
poly ek=reduce(k,std(0));
ek;
tst_status(1);$
