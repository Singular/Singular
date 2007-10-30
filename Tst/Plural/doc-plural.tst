LIB "tst.lib";
tst_init();
ring r1=(0,Q),(x,y,z),Dp;
minpoly=Q^4+Q^2+1;
matrix C[3][3];
matrix D[3][3];
C[1,2]=Q2;
C[1,3]=1/Q2;
C[2,3]=Q2;
D[1,2]=-Q*z;
D[1,3]=1/Q*y;
D[2,3]=-Q*x; 
def S=ncalgebra(C,D);setring S;
// it is quantum deformation U'_q(so_3)
// where q=Q^2 specialized at the 3rd root of unity
S;
kill r1,S;
ring r2=0,(Xa,Xb,Xc,Ya,Yb,Yc,Ha,Hb),dp;
matrix d[8][8];
d[1,2]=-Xc; d[1,4]=-Ha; d[1,6]=Yb;
d[1,7]=2*Xa; d[1,8]=-Xa; d[2,5]=-Hb;
d[2,6]=-Ya; d[2,7]=-Xb; d[2,8]=2*Xb;
d[3,4]=Xb; d[3,5]=-Xa; d[3,6]=-Ha-Hb;
d[3,7]=Xc; d[3,8]=Xc; d[4,5]=Yc;
d[4,7]=-2*Ya; d[4,8]=Ya; d[5,7]=Yb;
d[5,8]=-2*Yb; d[6,7]=-Yc; d[6,8]=-Yc;
def S2=ncalgebra(1,d);setring S2;
// it is U(sl_3)
S2;
kill r2,S2;
ring r3=0,(a,b,c,d),lp;
matrix c[4][4];
c[1,2]=1; c[1,3]=3; c[1,4]=-2;
c[2,3]=-1; c[2,4]=-3; c[3,4]=1;
def S3=ncalgebra(c,0);setring S3;
// it is some quasi--commutative algebra
S3;
kill r3,S3;
ring r4=0,(t,u,v,w),dp;
def S4=ncalgebra(-1,0); setring S4;
// it is anticommutative algebra
S4;
kill r4,S4;
tst_status(1);$
