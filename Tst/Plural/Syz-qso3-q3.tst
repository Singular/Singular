LIB "tst.lib";
tst_init();
ring r=(0,Q),(x,y,z),dp;
minpoly=Q^4+Q^2+1;
matrix A[3][3];
matrix B[3][3];
int u,j;
for(u=1;u<=3;u++)
{
  for(j=u;j<=3;j++)
  {
    A[u,j]=1;
  }
}
A[1,2]=Q2;
A[1,3]=1/Q2;
A[2,3]=Q2;
B[1,2]=-Q*z;
B[1,3]=1/Q*y;
B[2,3]=-Q*x;
system("PLURAL",A,B);
poly Cq=Q^4*x2+y2+Q^4*z2+Q*(1-Q^4)*x*y*z;
poly C1=x^3+x;
poly C2=y^3+y;
poly C3=z^3+z;
ideal I=Cq,C1,C2,C3;
I=std(I);
module a=syz(I);
print(a);
kill a;
resolution F=nres(I,0);
F;
int b;string s;
for (b=1;b<=size(list(F));b++)
{s=print("Step:","%s");s=s+string(b);s;F[b];}
resolution G=mres(I,0);
G;
for (b=1;b<=size(list(G));b++)
{s=print("Step:","%s");s=s+string(b);s;G[b];}
tst_status(1);$
