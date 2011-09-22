LIB "tst.lib"; tst_init();
// consider the quantized Weyl algebra
ring r = (0,q),(x,d),Dp;
def RS=nc_algebra(q,1);
setring RS; RS;
list l = ringlist(RS);
l;
// now, change the relation d*x = q*x*d +1
// into the relation d*x=(q2+1)*x*d + q*d + 1
matrix S = l[5]; // matrix of coefficients
S[1,2] = q^2+1;
l[5] = S;
matrix T = l[6]; // matrix of polynomials
T[1,2] = q*d+1;
l[6] = T;
def rr = ring(l);
setring rr; rr;
tst_status(1);$
