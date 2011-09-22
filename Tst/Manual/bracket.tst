LIB "tst.lib"; tst_init();
ring r=(0,Q),(x,y,z),Dp;
minpoly=Q^2-Q+1;
matrix C[3][3];  matrix D[3][3];
C[1,2]=Q2;    C[1,3]=1/Q2;  C[2,3]=Q2;
D[1,2]=-Q*z;  D[1,3]=1/Q*y; D[2,3]=-Q*x;
def R=nc_algebra(C,D); setring R; R;
// this is a quantum deformation of U(so_3),
// where Q is a 6th root of unity
poly p=Q^4*x2+y2+Q^4*z2+Q*(1-Q^4)*x*y*z;
// p is the central element of the algebra
p=p^3; // any power of a central element is central
poly q=(x+Q*y+Q^2*z)^4;
// take q to be some big noncentral element
size(q); // check how many monomials are in big polynomial q
bracket(p,q); // check p*q=q*p
// a more common behaviour of the bracket follows:
bracket(x+Q*y+Q^2*z,z);
tst_status(1);$
