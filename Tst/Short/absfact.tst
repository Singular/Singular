LIB "tst.lib";
tst_init();

LIB "absfact.lib";

ring R = (0), (x,y), lp;
short= 0;
poly p = (-7*x^2 + 2*x*y^2 + 6*x + y^4 + 14*y^2 + 47)*(5x2+y2)^3*(x-y)^4;
def S = absFactorize(p) ;
setring(S);
short= 0;
absolute_factors;
setring R;
def T= absFactorizeBCG (p);
setring T;
short= 0;
absolute_factors;

kill R;
ring R = (0), (x,y), lp;
short= 0;
poly p = (5x2+y2);
def S = absFactorize(p) ;
setring(S);
short= 0;
absolute_factors;
setring R;
def T= absFactorizeBCG (p);
setring T;
short= 0;
absolute_factors;

kill R;
ring R = (0), (x,y), lp;
short= 0;
poly p = (5x2+y2);
absFactorize(p,77) ;
absFactorizeBCG (p,77);

kill R;
ring R = (0), (x,y), lp;
short= 0;
poly p = (5x4-5x2y2);
def S = absFactorize(p) ;
setring(S);
short= 0;
absolute_factors;
setring R;
def T= absFactorizeBCG (p);
setring T;
short= 0;
absolute_factors;

ring r=0,(x,t),dp;
poly p=x^4+(t^3-2t^2-2t)*x^3-(t^5-2t^4-t^2-2t-1)*x^2
         -(t^6-4t^5+t^4+6t^3+2t^2)*x+(t^6-4t^5+2t^4+4t^3+t^2);
def S = absFactorize(p,"s");
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p, "s");
setring T;
absolute_factors;

kill r;
ring r=0,(x),dp;
poly p=0;
def S = absFactorize(p);
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p);
setring T;
absolute_factors;

kill r;
ring r=0,(x),dp;
poly p=7/11;
def S = absFactorize(p);
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p);
setring T;
absolute_factors;

kill r;
ring r=(0,a,b),(x,y),dp;
poly p=0;
def S = absFactorize(p);
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p);
setring T;
absolute_factors;

kill r;
ring r=(0,a,b),(x,y),dp;
poly p=(a+1)/b;
def S = absFactorize(p);
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p);
setring T;
absolute_factors;

kill r;
ring r=(0,a,b),(x,y),dp;
poly p=(a+1)/b*x;
def S = absFactorize(p,"s");
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p,"s");
setring T;
absolute_factors;

kill r;
ring r=(0,a,b),(x,y),dp;
poly p=(a+1)/b*x + 1;
def S = absFactorize(p,"s");
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p,"s");
setring T;
absolute_factors;

kill r;
ring r=(0,a,b),(x,y),dp;
poly p=(a+1)/b*x + y;
def S = absFactorize(p,"s");
setring(S);
absolute_factors;
setring r;
def T= absFactorizeBCG(p,"s");
setring T;
absolute_factors;

ring R1=0,(x,y),dp;
poly f=(x2+y2)^3*(x3+y2)^5*(x4+4)^2;
def S1 =absFactorize(f);
setring(S1);
absolute_factors;
setring R1;
def T= absFactorizeBCG(f);
setring T;
absolute_factors;

ring R3 = 0, (x,y), dp;
poly f3 = x2y4+y6+2x3y2+2xy4-7x4+7x2y2+14y4+6x3+6xy2+47x2+47y2;
def S=absFactorize (f3);
setring S;
absolute_factors;
setring R3;
def T=absFactorizeBCG (f3);
setring T;
absolute_factors;

ring r2=0,(x,y,z,w),dp;
poly f=(x2+y2+z2)^2+w4;
def S =absFactorize(f);
setring(S);
absolute_factors;
setring r2;
def T=absFactorizeBCG (f);
setring T;
absolute_factors;

ring r1=(0,a,b),(x,y),dp;
poly p=(a3-a2b+27ab3-27b4)/(a+b5)*x2+(a2+27b3)*y;
def S = absFactorize(p);
setring(S);
absolute_factors;
setring r1;
def T=absFactorizeBCG (p);
setring T;
absolute_factors;

ring r3=0,(x,y,z,w),dp;
poly f=(x2+y2+z2)^4+w8;
tst_status();
def S =absFactorize(f);
tst_status();
setring(S);
absolute_factors;
setring r3;
tst_status();
def T=absFactorizeBCG (f);
tst_status();
setring T;
absolute_factors;

ring R = 0, (x,y,z), dp;
poly f = y4+z2-y2*(1-x2);
def S = absFactorize(f);
setring S;
absolute_factors;
setring R;
def T=absFactorizeBCG (f);
setring T;
absolute_factors;

tst_status(1);$
