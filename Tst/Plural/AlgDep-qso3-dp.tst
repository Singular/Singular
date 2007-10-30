LIB "tst.lib";
tst_init();
ring r=(0,Q),(a,b,c,d,x,y,z),dp;
minpoly=Q^4+Q^2+1;
matrix A[7][7];
matrix B[7][7];
int u,j;
for(u=1;u<=7;u++)
{
  for(j=u;j<=7;j++)
  {
    A[u,j]=1;
  }
}
A[5,6]=Q2;
A[5,7]=1/Q2;
A[6,7]=Q2;
B[5,6]=-Q*z;
B[5,7]=1/Q*y;
B[6,7]=-Q*x;
def S=ncalgebra(A,B);setring S;
poly Cq=Q^4*x2+y2+Q^4*z2+Q*(1-Q^4)*x*y*z;
poly C1=1/3*(x^3+x);
poly C2=1/3*(y^3+y);
poly C3=1/3*(z^3+z);
ideal I=a-Cq,b-C1,c-C2,d-C3;
I=std(I);
I;
I[5];
tst_status(1);$
