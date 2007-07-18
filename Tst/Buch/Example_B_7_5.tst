LIB "tst.lib";
tst_init();

// ==================================== example B.7.5 =======================

LIB"absfact.lib";  

ring R1=0,(x,y),dp;
poly f=(x2+y2)^3*(x3+y2)^5*(x4+4)^2;
def S1 =absFactorize(f);
setring(S1);
absolute_factors;

ring R2=0,(x,y,z,w),dp;
poly f=(x2+y2+z2)^2+w4;
def S2 =absFactorize(f);
setring(S2);
absolute_factors;

tst_status(1);$

