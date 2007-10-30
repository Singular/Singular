LIB "tst.lib";
tst_init();
ring r=(0,Q),(x,y,z),Dp;
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
// it is Quantum deformation of U(so_3).
poly p=Q^4*x2+y2+Q^4*z2+Q*(1-Q^4)*x*y*z;
// it is the central element  
p=p^3;
p;
poly q=(x+Q*y+Q^2*z)^4;
// it is big noncentral element
q;
system("bracket",p,q); 
tst_status(1);$
