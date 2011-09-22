LIB "tst.lib"; tst_init();
LIB "dmod.lib";
ring r   = 0,(x,y),ds;
poly f1  = x*y*(x+y);
ideal I1 = bernstein(f1)[1]; // a local Bernstein poly
I1;
minIntRoot(I1,0);
poly  f2  = x2-y3;
ideal I2  = bernstein(f2)[1];
I2;
minIntRoot(I2,0);
// now we illustrate the behaviour of factorize
// together with a global ordering
ring r2  = 0,x,dp;
poly f3   = 9*(x+2/3)*(x+1)*(x+4/3); //global b-polynomial of f1=x*y*(x+y)
ideal I3 = factorize(f3,1);
I3;
minIntRoot(I3,1);
// and a more interesting situation
ring  s  = 0,(x,y,z),ds;
poly  f  = x3 + y3 + z3;
ideal I  = bernstein(f)[1];
I;
minIntRoot(I,0);
tst_status(1);$
