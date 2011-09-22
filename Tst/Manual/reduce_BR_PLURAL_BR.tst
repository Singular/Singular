LIB "tst.lib"; tst_init();
ring r=(0,a),(e,f,h),Dp;
matrix d[3][3];
d[1,2]=-h;  d[1,3]=2e;  d[2,3]=-2f;
def R=nc_algebra(1,d);
setring R;
// this algebra is U(sl_2) over Q(a)
ideal I = e2, f2, h2-1;
I = std(I);
// print a compact presentation of I
print(matrix(I));
ideal J = e, h-a;
J = std(J);
// print a compact presentation of J
print(matrix(J));
poly z=4*e*f+h^2-2*h;
// z is the central element of U(sl_2)
reduce(z,I);  // the central character of I:
reduce(z,J); // the central character of J:
poly nz = z - NF(z,J); // nz will belong to J
reduce(nz,J);
reduce(I,J);
tst_status(1);$
