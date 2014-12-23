LIB "tst.lib";
tst_init();
ring r=0,(x,y,z),dp;
matrix d[3][3];
d[1,2]=-z;
d[1,3]=2x;
d[2,3]=-2y;
def S=nc_algebra(1,d); setring S;
int N=15;
poly f=1;
for(int n=0;n<=N;n++)
{
  f=f*(z+N-2*n);
}
f;
option(redSB);
option(redTail);
ideal i=x^(N+1),y^(N+1),f;
ideal k=std(i);
k;
ideal J=system("twostd",i);
J;
tst_status(1);$
