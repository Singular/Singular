LIB "tst.lib";
tst_init();

// resultant should not change its input arguments:
ring rb =(0,b),x,dp;
poly f = 2/3*x+7/5;
resultant (x,f,x);
f;

// Other example
f = 2/3x+5;
poly g = 7/11x-3/8;
resultant (f,g,x);
-7/11*subst(2/3x+5,x,3/8*11/7);  // check by hand (product formula for resultant)

f;
g;

f = x/b+1;
resultant (x,f,x);
f;

tst_status(1);$
