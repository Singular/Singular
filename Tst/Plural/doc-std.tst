LIB "tst.lib";
tst_init();
option(redSB);
option(redTail);
ring R=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=y;
d[2,3]=-x;
def S=ncalgebra(1,d); setring S; //U(so_3)
ideal I=x3+4x,y2-z2,z3+z;
I=std(I);
I;
kill R,S;
ring Rq3=(0,Q),(x,y,z),dp; //U'_q(so_3)
minpoly=Q^4+Q^2+1; // at the 3rd root of unity
matrix C[3][3];
matrix D[3][3];
C[1,2]=Q2;
C[1,3]=1/Q2;
C[2,3]=Q2;
D[1,2]=-Q*z;
D[1,3]=1/Q*y;
D[2,3]=-Q*x; 
def S2=ncalgebra(C,D); setring S2;
ideal J=x3+4x,y2-z2,z3+z;
J=std(J);
J;
tst_status(1);$
