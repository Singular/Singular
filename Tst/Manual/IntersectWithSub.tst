LIB "tst.lib"; tst_init();
LIB "ncdecomp.lib";
ring R=(0,a),(e,f,h),Dp;
matrix @d[3][3];
@d[1,2]=-h;   @d[1,3]=2e;   @d[2,3]=-2f;
def r = nc_algebra(1,@d); setring r; // parametric U(sl_2)
ideal I = e,h-a;
ideal C;
C[1] = h^2-2*h+4*e*f; // the center of U(sl_2)
ideal X = IntersectWithSub(I,C);
X;
ideal G = e*f, h; // the biggest comm. subalgebra of U(sl_2)
ideal Y = IntersectWithSub(I,G);
Y;
tst_status(1);$
