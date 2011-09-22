LIB "tst.lib"; tst_init();
LIB "paraplanecurves.lib";
ring R = 0,(x,y,z),dp;
poly f1 = 1/2*x^5+x^2*y*z^2+x^3*y*z+1/2*x*y^2*z^2-2*x*y^3*z+y^5;
def Rp1 = paraPlaneCurve(f1);
setring Rp1;
PARA;
setring R;
poly f2 = x6+3x4y2+3x2y4+y6-4x4z2-34x3yz2-7x2y2z2+12xy3z2+6y4z2;
f2 = f2+36x2z4+36xyz4+9y2z4;
def Rp2 = paraPlaneCurve(f2);
setring Rp2;
PARA;
tst_status(1);$
