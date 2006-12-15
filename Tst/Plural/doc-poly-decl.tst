LIB "tst.lib";
tst_init();
ring r = 32003,(x,y,z),dp;
ncalgebra(-1,1);
r;
// ring of some differential-like operators 
poly s1  = x3y2+151x5y+186xy6+169y9;
poly s2  = 1*x^2*y^2*z^2+3z8;
poly s3  = 5/4x4y2+4/5*x*y^5+2x2y2z3+y7+11x10;
int a,b,c,t=37,5,4,1;
poly f=3*x^a+x*y^(b+c)+t*x^a*y^b*z^c;
f;
short = 0;
f;
tst_status(1);$
                          
