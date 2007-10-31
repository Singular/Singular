LIB "tst.lib";
tst_init();
ring r=(0,a),(e,f,h),Dp;
matrix d[3][3];
d[1,2]=-h;
d[1,3]=2e;
d[2,3]=-2f;
def S=nc_algebra(1,d); setring S;
// it is parametric U(sl_2)
ideal I=e2,f2,h2-1;
I=std(I);
ideal J=e,h-a;
J=std(J);
poly z=4*e*f+h^2-2*h;
// z is the central element
// the central character of I:
NF(z,I);
// the central character of J:
NF(z,J);
tst_status(1);$
