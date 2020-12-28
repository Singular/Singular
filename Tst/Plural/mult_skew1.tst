LIB "tst.lib";
tst_init();
ring r=(0,q1,q2,q3),(x,y,z),Dp;
matrix C[3][3];
C[1,2]=q1;
C[1,3]=q2;
C[2,3]=q3;
def S=nc_algebra(C,0);setring S;
int N=15;
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
z^7*y^6*x^5;
tst_status(1);$
