// File: mult_lie1.tst
// multiplication tests in enveloping algebras of Lie algebras
LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
matrix D[3][3];
D[1,2]=-z;
D[1,3]=2x;
D[2,3]=-2y;
def S=nc_algebra(1,D);setring S;
int N=11;
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
       if (g!=0) {"Associativity Failed at:";i;j;k;f;}
    }
  }
}
z^7*y^6*x^5;
tst_status(1);
$
