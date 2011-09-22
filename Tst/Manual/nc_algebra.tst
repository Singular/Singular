LIB "tst.lib"; tst_init();
LIB "nctools.lib";
// ------- first example: C, D are matrices --------
ring r1 = (0,Q),(x,y,z),Dp;
minpoly = rootofUnity(6);
matrix C[3][3];
matrix D[3][3];
C[1,2]=Q2;   C[1,3]=1/Q2;  C[2,3]=Q2;
D[1,2]=-Q*z; D[1,3]=1/Q*y; D[2,3]=-Q*x;
def S=nc_algebra(C,D);
// this algebra is a quantum deformation U'_q(so_3),
// where Q is a 6th root of unity
setring S;S;
kill r1,S;
// ----- second example: number n=1, D is a matrix
ring r2=0,(Xa,Xb,Xc,Ya,Yb,Yc,Ha,Hb),dp;
matrix d[8][8];
d[1,2]=-Xc; d[1,4]=-Ha; d[1,6]=Yb;  d[1,7]=2*Xa;
d[1,8]=-Xa; d[2,5]=-Hb; d[2,6]=-Ya; d[2,7]=-Xb;
d[2,8]=2*Xb; d[3,4]=Xb; d[3,5]=-Xa; d[3,6]=-Ha-Hb;
d[3,7]=Xc;   d[3,8]=Xc; d[4,5]=Yc; d[4,7]=-2*Ya;
d[4,8]=Ya;  d[5,7]=Yb; d[5,8]=-2*Yb;
d[6,7]=-Yc; d[6,8]=-Yc;
def S=nc_algebra(1,d);  // this algebra is U(sl_3)
setring S;S;
kill r2,S;
// ---- third example: C is a matrix, p=0 is a poly
ring r3=0,(a,b,c,d),lp;
matrix c[4][4];
c[1,2]=1; c[1,3]=3; c[1,4]=-2;
c[2,3]=-1; c[2,4]=-3; c[3,4]=1;
def S=nc_algebra(c,0); // it is a quasi--commutative algebra
setring S;S;
kill r3,S;
// -- fourth example : number n = -1, poly p = 3w
ring r4=0,(u,v,w),dp;
def S=nc_algebra(-1,3w);
setring S;S;
kill r4,S;
tst_status(1);$
