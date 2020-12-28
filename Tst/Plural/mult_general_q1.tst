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
def S=nc_algebra(C,D); setring S;
int N=5;
poly f;
poly g;
int i;
int j;
int k;
for(i=1;i<=N;i++)
{
  for(j=1;j<=N;j++)
  {
    for(k=1;k<=N;k++)
    {
       f=(z^k*y^j)*x^i;
       g=z^k*(y^j*x^i);
       g=g-f;
       if (g!=0) {"Fail";i;j;k;f;}
    }
  }
}
z^5*y^4*x^3;
tst_status(1);$
